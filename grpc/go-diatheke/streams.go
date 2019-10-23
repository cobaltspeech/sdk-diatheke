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
	"fmt"

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
)

// AudioInputStream allows audio to be pushed to the server for a session.
type AudioInputStream struct {
	PBStream diathekepb.Diatheke_StreamAudioInputClient
}

// Write the given audio data to the stream.
func (ais *AudioInputStream) Write(p []byte) (n int, err error) {

	// Setup the request
	req := diathekepb.AudioInput{
		Request: &diathekepb.AudioInput_Data{
			Data: p,
		},
	}

	// Send it on the stream.
	err = ais.PBStream.Send(&req)
	if err != nil {
		return 0, err
	}

	return len(p), nil
}

// Finish notifies Diatheke that no more audio is coming
func (ais *AudioInputStream) Finish() error {
	_, err := ais.PBStream.CloseAndRecv()
	return err
}

// ASRStream is a bi-directional stream that allows audio data to be pushed
// to the server, while transcription results are streamed back to the client.
// The audio and transcripts are unrelated to any running sessions.
type ASRStream struct {
	PBStream diathekepb.Diatheke_StreamASRClient
}

// Write the given audio data to the stream
func (asr *ASRStream) Write(p []byte) (n int, err error) {
	return 0, fmt.Errorf("not implemented")
}

// Receive waits for a transcript from the server.
func (asr *ASRStream) Receive() error {
	return fmt.Errorf("not implemented")
}
