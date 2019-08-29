// Copyright (2019) Cobalt Speech and Language Inc.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Package diatheke provides for interacting with an instance of diatheke server using
// GRPC for performing speech recognition.
package diatheke

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io"
	"log"
	"sync"

	"github.com/cobaltspeech/diatheke/sdk-diatheke/grpc/go-diatheke/diathekepb"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials"
)

// Client is an object for interacting with the Diatheke gRPC API.
//
// All methods except Close may be called concurrently.
type Client struct {
	conn             *grpc.ClientConn
	diathekePBClient diathekepb.DiathekeClient
	insecure         bool
	tlscfg           tls.Config
	streamingBufSize uint32
}

// GetInternalDiathekePBClient returns the internal diatheke protobuf client.
// This is useful for making lower-level calls, such as pushing audio directly
// instead of using the higher level PushAudio(io.Reader) call.
func (c *Client) GetInternalDiathekePBClient() diathekepb.DiathekeClient {
	return c.diathekePBClient
}

// NewClient creates a new Client that connects to a Diatheke Server listening on
// the provided address.  Transport security is enabled by default.  Use Options
// to override default settings if necessary.
func NewClient(addr string, opts ...Option) (*Client, error) {
	c := Client{}
	c.streamingBufSize = defaultStreamingBufsize

	for _, opt := range opts {
		err := opt(&c)
		if err != nil {
			return nil, fmt.Errorf("unable to create a client: %v", err)
		}
	}

	var dopt grpc.DialOption

	if c.insecure {
		dopt = grpc.WithInsecure()
	} else {
		dopt = grpc.WithTransportCredentials(credentials.NewTLS(&c.tlscfg))
	}

	conn, err := grpc.Dial(addr, dopt)
	if err != nil {
		return nil, fmt.Errorf("unable to create a client: %v", err)
	}
	c.conn = conn
	c.diathekePBClient = diathekepb.NewDiathekeClient(c.conn)
	return &c, nil
}

// Option configures how we setup the connection with a server.
type Option func(*Client) error

// WithInsecure returns an Option which disables transport security for this
// Client.  Use this when connecting to a non-TLS enabled diatheke server, such as
// during debugging.
func WithInsecure() Option {
	return func(c *Client) error {
		c.insecure = true
		return nil
	}
}

// WithServerCert returns an Option which sets up the given PEM certificate as a
// root certificate that can validate the certificate presented by the server we
// are connecting to.  Use this when connecting to an instance of diatheke server
// that is using a self-signed certificate.
func WithServerCert(cert []byte) Option {
	return func(c *Client) error {
		caCertPool := x509.NewCertPool()
		if ok := caCertPool.AppendCertsFromPEM(cert); !ok {
			return fmt.Errorf("unable to use given caCert")
		}
		c.tlscfg.RootCAs = caCertPool
		return nil
	}
}

// WithClientCert returns an Option which sets up the given PEM certificate and
// key as the credentials presented by this Client when connecting to a server.
// Use this when setting up mutually authenticated TLS.
func WithClientCert(certPem []byte, keyPem []byte) Option {
	return func(c *Client) error {
		clientCert, err := tls.X509KeyPair(certPem, keyPem)
		if err != nil {
			return err
		}

		c.tlscfg.Certificates = []tls.Certificate{clientCert}
		return nil
	}
}

// WithStreamingBufferSize returns an Option that sets up the buffer size
// (bytes) of each message sent from the Client to the server during streaming
// GRPC calls.  Use this only if Cobalt recommends you to do so.  A value n>0 is
// required.
func WithStreamingBufferSize(n uint32) Option {
	return func(c *Client) error {
		if n == 0 {
			return fmt.Errorf("invalid streaming buffer size of 0")
		}
		c.streamingBufSize = n
		return nil
	}
}

// Close closes the connection to the API service.  The user should only invoke
// this when the client is no longer needed.  Pending or in-progress calls to
// other methods may fail with an error if Close is called, and any subsequent
// calls with this client will also fail.
func (c *Client) Close() error {
	return c.conn.Close()
}

// Version queries the server for its version
func (c *Client) Version(ctx context.Context, opts ...grpc.CallOption) (*diathekepb.VersionResponse, error) {
	return c.diathekePBClient.Version(ctx, &diathekepb.Empty{}, opts...)
}

// NewSession creates a new session and returns the session id.
func (c *Client) NewSession(ctx context.Context, request *diathekepb.NewSessionRequest, opts ...grpc.CallOption) (*diathekepb.NewSessionResponse, error) {
	return c.diathekePBClient.NewSession(ctx, request, opts...)
}

// EndSession ends an existing session.
func (c *Client) EndSession(ctx context.Context, request *diathekepb.SessionEndRequest, opts ...grpc.CallOption) (*diathekepb.Empty, error) {
	return c.diathekePBClient.EndSession(ctx, request, opts...)
}

const defaultStreamingBufsize uint32 = 8192

// TranscriptionResultHandler is a type of callback function that will be called
// when the `PushAudio` method is running.  For each response received
// from diatheke server, this method will be called once.  The provided
// TranscriptionResult is guaranteed to be non-nil.  Since this function is
// executed as part of the streaming process, it should preferably return
// quickly and certainly not block.
type TranscriptionResultHandler func(*diathekepb.TranscriptionResult)

// PushAudio uses the bidirectional streaming API for performing speech
// recognition.
//
// Data is read from the given audio reader into a buffer and streamed to diatheke
// server.  The default buffer size may be overridden using Options when
// creating the Client.
//
// As results are received from the diatheke server, they will be sent to the
// provided handlerFunc.
//
// If any error occurs while reading the audio or sending it to the server, this
// method will immediately exit, returning that error.
//
// This function returns only after all results have been passed to the
// resultHandler.
//
// You may close the audio reader if you want to endpoint the audio, but will
// need to call the function again for future calls.
func (c *Client) PushAudio(ctx context.Context, sessionID string, audio io.Reader,
	handlerFunc TranscriptionResultHandler, opts ...grpc.CallOption) error {

	stream, err := c.diathekePBClient.PushAudio(ctx, opts...)
	if err != nil {
		return fmt.Errorf("unable to start streaming recognition: %v", err)
	}

	// There are two concurrent processes going on.  We will create a new
	// goroutine to read audio and stream it to the server.  This goroutine
	// will receive results from the stream.  Errors could occur in both
	// goroutines.  We therefore setup a channel, errch, to hold these
	// errors. Both goroutines are designed to send up to one error, and
	// return immediately. Therefore we use a bufferred channel with a
	// capacity of two.
	errch := make(chan error, 2)

	// start streaming audio in a separate goroutine
	var wg sync.WaitGroup
	wg.Add(1)
	go func() {
		if err := sendaudio(stream, sessionID, audio, c.streamingBufSize); err != nil && err != io.EOF {
			// if sendaudio encountered io.EOF, it's only a
			// notification that the stream has closed.  The actual
			// status will be obtained in a subsequent Recv call, in
			// the other goroutine below.  We therefore only forward
			// non-EOF errors.
			errch <- err
		}
		stream.CloseSend()
		wg.Done()
	}()

	for {
		in, err := stream.Recv()
		if err == io.EOF {
			break
		}
		if err != nil {
			errch <- err
			break
		}
		handlerFunc(in)
	}

	wg.Wait()

	select {
	case err := <-errch:
		// There may be more than one error in the channel, but it is
		// very likely they are related (e.g. connection reset causing
		// both the send and recv to fail) and we therefore return the
		// first error and discard the other.
		return fmt.Errorf("streaming recognition failed: %v", err)
	default:
		return nil
	}
}

// sendaudio sends audio to a stream.
func sendaudio(stream diathekepb.Diatheke_PushAudioClient,
	sessionID string, audio io.Reader,
	bufsize uint32) error {

	// The first message needs to be a config message, and all subsequent
	// messages must be audio messages.

	// Send the recogniton config
	if err := stream.Send(&diathekepb.AudioTranscriptionRequest{
		Request: &diathekepb.AudioTranscriptionRequest_SessionId{SessionId: sessionID},
	}); err != nil {
		// if this failed, we don't need to CloseSend
		return err
	}

	// Stream the audio.
	buf := make([]byte, bufsize)
	for {
		n, err := audio.Read(buf)

		if err2 := stream.Send(&diathekepb.AudioTranscriptionRequest{
			Request: &diathekepb.AudioTranscriptionRequest_Data{
				Data: buf[:n],
			},
		}); err2 != nil {
			// if we couldn't Send, the stream has
			// encountered an error and we don't need to
			// CloseSend.
			return err2
		}

		if err != nil {
			// err could be io.EOF, or some other error reading from
			// audio.  In any case, we need to CloseSend, send the
			// appropriate error to errch and return from the function
			if err2 := stream.CloseSend(); err2 != nil {
				return err2
			}
			if err != io.EOF {
				return err
			}
			return nil
		}
	}
}

// PushText sends text for the Dialog management to process.
func (c *Client) PushText(ctx context.Context, request *diathekepb.PushTextRequest, opts ...grpc.CallOption) (*diathekepb.Empty, error) {
	return c.diathekePBClient.PushText(ctx, request, opts...)
}

// CommandAndNotify uses the server streaming API for recieving CommandToExecute
// as the Dialog management engine processes text or transcribed audio inputs.
// As actions are identitfied from the diatheke server, they will be sent to the
// provided handlerFunc.  Your application manager should execute the commands,
// and call Notify(CommandStatusUpdate) to provide feedback and update the dialog
// state.
//
// If any error occurs while reading the audio or sending it to the server, this
// method will immediately exit, returning that error.
//
// This function does not return, unless there is an error. If the context was
// canceled, we return an error saying this function was canceled, as opposed
// to the server finished up without error.  Ending the session should cause the
// server to close the stream and allow this function to return.
func (c *Client) CommandAndNotify(ctx context.Context, opts ...grpc.CallOption) (diathekepb.Diatheke_CommandAndNotifyClient, error) {
	return c.diathekePBClient.CommandAndNotify(ctx, opts...)
}

// TTSResponseHandler is a type of callback function that will be called
// when the `SayCallback` method is running.  For each response received
// from diatheke server, this method will be called once.  The provided
// TTSResponse is guaranteed to be non-nil.  Since this function is
// executed as part of the streaming process, it should preferably return
// quickly and certainly not block.
type TTSResponseHandler func(*diathekepb.TTSResponse)

// SayCallback uses the server streaming API for recieving TTSResponses as the TTS
// engine finishes up TTS synthesis.  As results are received from the diatheke
// server, they will be sent to the provided handlerFunc.
//
// If any error occurs while reading the audio or sending it to the server, this
// method will immediately exit, returning that error.
//
// This function does not return, unless there is an error. If the context was
// canceled, we return an error saying this function was canceled, as opposed
// to the server finished up without error.  Ending the session should cause the
// server to close the stream and allow this function to return.
func (c *Client) SayCallback(ctx context.Context, sessionID string, handlerFunc TTSResponseHandler, opts ...grpc.CallOption) error {
	stream, err := c.diathekePBClient.SayCallback(ctx, &diathekepb.SayCallbackRequest{SessionId: sessionID}, opts...)
	if err != nil {
		return fmt.Errorf("unable to start command callback: %v", err)
	}

	fail := make(chan error, 1)
	inputChan := make(chan *diathekepb.TTSResponse, 1)
	// Read the results from the server, place it into a channel for consumption
	// in the same switch statement as the context check.
	go func() {
		for {
			m, err := stream.Recv()
			if err == io.EOF {
				close(fail)
				close(inputChan)
				return
			}
			if err != nil {
				fail <- err
				close(fail)
				close(inputChan)
				return
			}
			inputChan <- m
		}
	}()

	// Read stream of results
	for {
		select {
		case in := <-inputChan:
			log.Printf("**Listening for say callback -- action\n")
			handlerFunc(in)
		case err := <-fail:
			return fmt.Errorf("Error getting TTS Response: %v", err)
		case <-ctx.Done():
			return fmt.Errorf("command callback was canceled by the user")
		}
	}
}

// Say sends text for the TTS engine to synthesize.
func (c *Client) Say(ctx context.Context, request *diathekepb.TTSRequest, opts ...grpc.CallOption) (*diathekepb.Empty, error) {
	return c.diathekePBClient.Say(ctx, request, opts...)
}
