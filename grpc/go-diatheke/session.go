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

package diatheke

import (
	"context"

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
)

// Session represents a Diatheke session. It is a wrapper around
// diatheke.Client, provided as a convenience for working with
// methods that require a session ID.
type Session struct {
	ID     string
	Parent *Client
}

// Start tells Diatheke to begin executing the model associated
// with this session.
func (sess *Session) Start(ctx context.Context) error {
	return sess.Parent.StartSession(ctx, sess.ID)
}

// EndSession ends this session.
func (sess *Session) EndSession(ctx context.Context) error {
	return sess.Parent.EndSession(ctx, sess.ID)
}

// EventStream returns a new event stream for this session.
func (sess *Session) EventStream(ctx context.Context) (diathekepb.Diatheke_SessionEventStreamClient, error) {
	return sess.Parent.SessionEventStream(ctx, sess.ID)
}

// CommandFinished notifies the server that a command has completed. This
// should be called after receiving a command event in the session's
// event stream, as required by the Diatheke model.
func (sess *Session) CommandFinished(ctx context.Context, status *diathekepb.CommandStatus) error {
	// Ensure the session ID matches the one for this session.
	if status != nil {
		status.SessionId = sess.ID
	}

	return sess.Parent.CommandFinished(ctx, status)
}

// StreamAudioInput returns a stream object that may be used to push audio
// data to the Diatheke server for this session. Only one stream per
// session should be running concurrently.
func (sess *Session) StreamAudioInput(ctx context.Context) (*AudioInputStream, error) {
	return sess.Parent.StreamAudioInput(ctx, sess.ID)
}

// StreamAudioReplies returns a stream object that receives output audio from
// Diatheke specifically for this session. The stream will include start
// and end messages to indicate when a section of audio for a group of text
// begins and ends.
func (sess *Session) StreamAudioReplies(ctx context.Context) (diathekepb.Diatheke_StreamAudioRepliesClient, error) {
	return sess.Parent.StreamAudioReplies(ctx, sess.ID)
}

// PushText sends the given text to Diatheke as part of a conversation for
// this session.
func (sess *Session) PushText(ctx context.Context, text string) error {
	return sess.Parent.PushText(ctx, sess.ID, text)
}

// SetStory changes the current story of a session to the one specified.
// Stories are defined by the Diatheke model.
func (sess *Session) SetStory(ctx context.Context, storyRequest *diathekepb.StoryRequest) error {
	// Ensure the session ID matches the one for this session.
	if storyRequest != nil {
		storyRequest.SessionId = sess.ID
	}

	return sess.Parent.SetStory(ctx, storyRequest)
}
