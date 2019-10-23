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
// gRPC.
package diatheke

import (
	"context"

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
)

// Session represents a Diatheke session. It is provided as a convenience for
// working with methods that require a session ID.
type Session struct {
	ID     string
	Parent *Client
}

// EndSession ends this session.
func (sess *Session) EndSession(ctx context.Context) error {
	return sess.Parent.EndSession(ctx, sess.ID)
}

// EventStream returns a new event stream for this session
func (sess *Session) EventStream(ctx context.Context) (diathekepb.Diatheke_SessionEventStreamClient, error) {
	return sess.Parent.SessionEventStream(ctx, sess.ID)
}

// CommandFinished notifies Diatheke that a command has completed. The initial
// command request will come as part of a DiathekeEvent in a session's event
// stream. This method should always be called after receiving a command event
// when the command is completed, even if it is later (e.g., long running
// commands).
func (sess *Session) CommandFinished(ctx context.Context, status *diathekepb.CommandStatus) error {
	return sess.Parent.CommandFinished(ctx, status)
}

// StreamAudioInput creates a stream to use to push audio input to Diatheke,
// specifically for this session.
func (sess *Session) StreamAudioInput(ctx context.Context) (*AudioInputStream, error) {
	return sess.Parent.StreamAudioInput(ctx, sess.ID)
}

// StreamAudioReplies creates a stream to receive audio output from Diatheke,
// specifically for this session.
func (sess *Session) StreamAudioReplies(ctx context.Context) (diathekepb.Diatheke_StreamAudioRepliesClient, error) {
	return sess.Parent.StreamAudioReplies(ctx, sess.ID)
}

// PushText to Diatheke as part of a conversation for this session.
func (sess *Session) PushText(ctx context.Context, text string) error {
	return sess.Parent.PushText(ctx, sess.ID, text)
}
