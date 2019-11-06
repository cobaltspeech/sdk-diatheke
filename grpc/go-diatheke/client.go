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

// Package diatheke provides an interface to interact with Diatheke
// server using gRPC.
package diatheke

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"fmt"

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials"
)

// Client is an object for interacting with the Diatheke gRPC API.
//
// All methods except Close may be called concurrently.
type Client struct {
	// The protobuf-defined client. Most users will not need to call this
	// directly, but is exposed as a convenience for advanced users who
	// wish to use gRPC functionality beyond what this interface provides.
	PBClient diathekepb.DiathekeClient

	// The list of gRPC call options that are used when the client
	// makes server requests.
	CallOpts []grpc.CallOption

	// Internal data
	conn     *grpc.ClientConn
	insecure bool
	tlscfg   tls.Config
}

// NewClient creates a new Client that connects to a Diatheke Server listening on
// the provided address.  Transport security is enabled by default.  Use Options
// to override default settings if necessary.
func NewClient(addr string, opts ...Option) (*Client, error) {
	c := Client{}

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
	c.PBClient = diathekepb.NewDiathekeClient(c.conn)
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

// Close closes the connection to the API service.  The user should only invoke
// this when the client is no longer needed.  Pending or in-progress calls to
// other methods may fail with an error if Close is called, and any subsequent
// calls with this client will also fail.
func (c *Client) Close() error {
	return c.conn.Close()
}

// SetCallOptions replaces any current gRPC call options with the given set
// to use when making server requests.
func (c *Client) SetCallOptions(opts ...grpc.CallOption) {
	newOpts := make([]grpc.CallOption, 0)
	for _, o := range opts {
		newOpts = append(newOpts, o)
	}

	c.CallOpts = newOpts
}

// AppendCallOptions adds the given gRPC call options to the current
// list of options to use when making server requests. It does not
// check to see if the options are unique in the final list.
func (c *Client) AppendCallOptions(opts ...grpc.CallOption) {
	for _, o := range opts {
		c.CallOpts = append(c.CallOpts, o)
	}
}

// DiathekeVersion queries the Diatheke server for its version.
func (c *Client) DiathekeVersion(ctx context.Context) (string, error) {
	response, err := c.PBClient.Version(ctx, &diathekepb.Empty{}, c.CallOpts...)
	if err != nil {
		return "", err
	}

	return response.Server, nil
}

// ListModels queries the server for the list of currently loaded
// Diatheke models.
func (c *Client) ListModels(ctx context.Context) ([]string, error) {
	response, err := c.PBClient.Models(ctx, &diathekepb.Empty{}, c.CallOpts...)
	if err != nil {
		return nil, err
	}

	return response.Models, nil
}

// NewSession creates a new session and returns the session id.
func (c *Client) NewSession(ctx context.Context, model string) (string, error) {
	request := diathekepb.NewSessionRequest{
		Model: model,
	}

	response, err := c.PBClient.NewSession(ctx, &request, c.CallOpts...)
	if err != nil {
		return "", err
	}

	return response.SessionId, nil
}

// EndSession ends an existing session.
func (c *Client) EndSession(ctx context.Context, sessionID string) error {
	request := diathekepb.SessionID{
		SessionId: sessionID,
	}
	_, err := c.PBClient.EndSession(ctx, &request, c.CallOpts...)
	return err
}

// SessionEventStream returns a new event stream for the given session ID.
func (c *Client) SessionEventStream(ctx context.Context, sessionID string) (diathekepb.Diatheke_SessionEventStreamClient, error) {
	request := diathekepb.SessionID{
		SessionId: sessionID,
	}

	return c.PBClient.SessionEventStream(ctx, &request, c.CallOpts...)
}

// CommandFinished notifies the server that a command has completed. This
// should be called after receiving a command event in the session's
// event stream, as required by the Diatheke model.
func (c *Client) CommandFinished(ctx context.Context, commandStatus *diathekepb.CommandStatus) error {
	_, err := c.PBClient.CommandFinished(ctx, commandStatus, c.CallOpts...)
	return err
}

// StreamAudioInput returns a stream object that may be used to push audio
// data to the Diatheke server for the given session. Only one stream per
// session should be running concurrently.
func (c *Client) StreamAudioInput(ctx context.Context, sessionID string) (*AudioInputStream, error) {
	// First create the stream
	stream, err := c.PBClient.StreamAudioInput(ctx, c.CallOpts...)
	if err != nil {
		return nil, err
	}

	// Make sure the session ID is sent first
	idMessage := diathekepb.AudioInput{
		Request: &diathekepb.AudioInput_SessionId{
			SessionId: sessionID,
		},
	}

	if err := stream.Send(&idMessage); err != nil {
		return nil, err
	}

	// Return the stream in a wrapper, which is now ready to push audio data.
	return &AudioInputStream{stream}, nil
}

// StreamAudioReplies returns a stream object that receives output audio from
// Diatheke specifically for the given session. The stream will include start
// and end messages to indicate when a section of audio for a group of text
// begins and ends.
func (c *Client) StreamAudioReplies(ctx context.Context, sessionID string) (diathekepb.Diatheke_StreamAudioRepliesClient, error) {
	// Create the stream
	req := diathekepb.SessionID{
		SessionId: sessionID,
	}
	return c.PBClient.StreamAudioReplies(ctx, &req, c.CallOpts...)
}

// PushText sends the given text to Diatheke as part of a conversation for
// the given session.
func (c *Client) PushText(ctx context.Context, sessionID, text string) error {
	// Create the request and send it
	req := diathekepb.PushTextRequest{
		SessionId: sessionID,
		Text:      text,
	}

	_, err := c.PBClient.PushText(ctx, &req, c.CallOpts...)
	return err
}

// StreamASR runs streaming speech recognition unrelated to a session, using
// the specified ASR model.
func (c *Client) StreamASR(ctx context.Context, model string) (*ASRStream, error) {
	// Create the stream.
	stream, err := c.PBClient.StreamASR(ctx, c.CallOpts...)
	if err != nil {
		return nil, err
	}

	// Send the first message, which contains the model ID
	req := diathekepb.ASRRequest{
		AsrData: &diathekepb.ASRRequest_Model{
			Model: model,
		},
	}

	if err := stream.Send(&req); err != nil {
		return nil, err
	}

	wrapper := &ASRStream{
		PBStream: stream,
	}

	return wrapper, nil
}

// StreamTTS runs streaming text-to-speech unrelated to a session. It
// synthesizes speech for the given text, using the specified TTS model.
// To create a stream that can be cancelled by the client, use the
// context.WithCancel() function to create a new context and
// context.CancelFunc. Pass the new context to this function, and the
// stream will end when the corresponding CancelFunc is called.
func (c *Client) StreamTTS(ctx context.Context, model, text string) (diathekepb.Diatheke_StreamTTSClient, error) {
	// Setup the TTS request
	req := diathekepb.TTSRequest{
		Model: model,
		Text:  text,
	}

	// Create the stream
	return c.PBClient.StreamTTS(ctx, &req, c.CallOpts...)
}
