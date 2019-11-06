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
)

// RecordAudio demonstrates creating an AudioInput stream for a session.
func (manager *sessionManager) recordAudio() {
	// Create the input stream for the session.
	stream, err := manager.Session.StreamAudioInput(context.Background())
	if err != nil {
		textui.PrintError("Error creating session ASR stream: %v\n", err)
		return
	}

	// Let Diatheke know that no more input is coming when this function ends
	defer func() {
		if err := stream.Finish(); err != nil && err != io.EOF {
			textui.PrintError("Error closing session ASR stream: %v\n", err)
		}
	}()

	// Start the recording application
	rec := audio.NewRecorder(manager.RecordingConfig)
	if err := rec.Start(); err != nil {
		textui.PrintError("Failed to start recording app: %v\n", err)
		return
	}

	// Make sure stop the recording app when we exit
	defer rec.Stop()

	// Create an 8kB buffer to hold the audio
	buffer := make([]byte, 8192)

	// Send the recorded audio data to Diatheke for as long as the
	// recording flag is set
	for manager.IsRecording() {
		// Read the audio
		bytesRead, err := rec.Read(buffer)
		if err != nil {
			textui.PrintError("error reading audio data: %v\n", err)
			continue
		}

		// If playback is currently running, it is possible that the TTS audio
		// could be overheard in the recording. One (not very robust) way to
		// handle that issue is to mute recording during playback, which we
		// essentially do here by not forwarding the audio.
		if manager.IsPlaying() {
			continue
		}

		// Send the audio to Diatheke
		if _, err := stream.Write(buffer[0:bytesRead]); err != nil {
			textui.PrintError("error pushing audio data: %v\n", err)
		}
	}
}
