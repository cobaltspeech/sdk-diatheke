// Copyright (2020) Cobalt Speech and Language Inc.

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

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/v2/diathekepb"
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

// Version returns version information from the server.
func (c *Client) Version(ctx context.Context) (*diathekepb.VersionResponse, error) {
	return c.PBClient.Version(ctx, &diathekepb.Empty{}, c.CallOpts...)
}

// ListModels returns a list of Diatheke models available
// to the server.
func (c *Client) ListModels(ctx context.Context) (*diathekepb.ListModelsResponse, error) {
	return c.PBClient.ListModels(ctx, &diathekepb.Empty{}, c.CallOpts...)
}

// CreateSession creates a new Diatheke session using the specified model.
func (c *Client) CreateSession(
	ctx context.Context, model string) (*diathekepb.SessionOutput, error) {
	req := diathekepb.SessionStart{
		ModelId: model,
	}

	return c.PBClient.CreateSession(ctx, &req, c.CallOpts...)
}

// DeleteSession cleans up the given token. Behavior is undefined
// if the given token is used again after calling this function.
func (c *Client) DeleteSession(
	ctx context.Context, token *diathekepb.TokenData) error {
	_, err := c.PBClient.DeleteSession(ctx, token, c.CallOpts...)
	return err
}

// ProcessText sends the given text to Diatheke and returns an updated
// session token.
func (c *Client) ProcessText(
	ctx context.Context, token *diathekepb.TokenData, text string,
) (*diathekepb.SessionOutput, error) {
	req := diathekepb.SessionInput{
		Token: token,
		Input: &diathekepb.SessionInput_Text{
			Text: &diathekepb.TextInput{
				Text: text,
			},
		},
	}

	return c.PBClient.UpdateSession(ctx, &req, c.CallOpts...)
}

// ProcessASRResult sends the given ASR result to Diatheke and returns an
// updated session token.
func (c *Client) ProcessASRResult(
	ctx context.Context,
	token *diathekepb.TokenData,
	result *diathekepb.ASRResult,
) (*diathekepb.SessionOutput, error) {
	req := diathekepb.SessionInput{
		Token: token,
		Input: &diathekepb.SessionInput_Asr{
			Asr: result,
		},
	}

	return c.PBClient.UpdateSession(ctx, &req, c.CallOpts...)
}

// ProcessCommandResult sends the given command result to Diatheke and
// returns an updated session token. This function should be called in
// response to a command action Diatheke sent previously.
func (c *Client) ProcessCommandResult(
	ctx context.Context,
	token *diathekepb.TokenData,
	result *diathekepb.CommandResult,
) (*diathekepb.SessionOutput, error) {
	req := diathekepb.SessionInput{
		Token: token,
		Input: &diathekepb.SessionInput_Cmd{
			Cmd: result,
		},
	}

	return c.PBClient.UpdateSession(ctx, &req, c.CallOpts...)
}

// SetStory changes the current story for a Diatheke session. Returns
// and updated session token.
func (c *Client) SetStory(
	ctx context.Context,
	token *diathekepb.TokenData,
	storyID string,
	params map[string]string,
) (*diathekepb.SessionOutput, error) {
	req := diathekepb.SessionInput{
		Token: token,
		Input: &diathekepb.SessionInput_Story{
			Story: &diathekepb.SetStory{
				StoryId:    storyID,
				Parameters: params,
			},
		},
	}

	return c.PBClient.UpdateSession(ctx, &req, c.CallOpts...)
}

// NewSessionASRStream creates a new stream to transcribe audio
// for the given session Token.
func (c *Client) NewSessionASRStream(
	ctx context.Context,
	token *diathekepb.TokenData,
) (*ASRStream, error) {
	// Create the stream
	pbStream, err := c.PBClient.StreamASR(ctx, c.CallOpts...)
	if err != nil {
		return nil, err
	}

	// Wrap it in our class and send the token.
	stream := ASRStream{
		PBStream: pbStream,
	}
	return &stream, stream.SendToken(token)
}

// NewTTSStream creates a new stream to receive TTS audio from Diatheke
// based on the given ReplyAction.
func (c *Client) NewTTSStream(
	ctx context.Context,
	reply *diathekepb.ReplyAction,
) (*TTSStream, error) {
	// Create the stream
	pbStream, err := c.PBClient.StreamTTS(ctx, reply, c.CallOpts...)
	if err != nil {
		return nil, err
	}

	// Wrap it in our class
	return &TTSStream{
		PBStream: pbStream,
	}, err
}
