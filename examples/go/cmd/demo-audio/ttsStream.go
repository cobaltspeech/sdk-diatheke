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

package main

import (
	"context"
	"io"

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/audio"
	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/textui"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
)

// HandleTTSReplies demonstrates how to use the audio reply stream for
// a session. (TTS = Text To Speech)
func (manager *sessionManager) handleTTSReplies() {
	// Create the stream
	stream, err := manager.Session.StreamAudioReplies(context.Background())
	if err != nil {
		textui.PrintError("Error creating session TTS stream: %v\n", err)
		return
	}

	// Setup the playback application
	player := audio.NewPlayer(manager.PlaybackConfig)

	for {
		// Wait for messages from the server until the stream is closed,
		// which will happen when the session is closed or the stream's
		// context ends.
		msg, err := stream.Recv()

		// This indicates the stream has finished, which will happen
		// when the session ends or the context used to create the stream
		// closes.
		if err == io.EOF {
			break
		}
		if err != nil {
			textui.PrintError("TTS reply stream error: %v\n", err)
			return
		}

		switch reply := msg.OutputMessage.(type) {
		case *diathekepb.AudioReply_Text:
			// This message contains the text of the reply, which comes
			// at the beginning of TTS. Start the playback app so it is
			// ready to play audio.
			if err := player.Start(); err != nil {
				textui.PrintError("failed to start playback app: %v\n", err)
			}
			manager.SetPlaying(true)

		case *diathekepb.AudioReply_Data:
			// This message contains audio data. We receive this message
			// until TTS synthesis is complete. Send the data to the
			// playback app. The data format is specified in the Diatheke
			// server configuration.
			if err := player.PushAudio(reply.Data); err != nil {
				textui.PrintError("failed to push playback audio: %v\n", err)
			}

		case *diathekepb.AudioReply_End:
			// This message notifies us that TTS has finished. Stop
			// the playback application.
			if err := player.Stop(); err != nil {
				textui.PrintError("failed to stop playback audio: %v\n", err)
			}
			manager.SetPlaying(false)

		default:
			textui.PrintError("received unexpected message in AudioReply stream\n")
		}
	}
}
