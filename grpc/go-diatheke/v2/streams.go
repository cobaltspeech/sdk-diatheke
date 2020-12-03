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

package diatheke

import (
	"io"

	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/v2/diathekepb"
)

// ASRStream represents a stream of data sent from the client to
// Diatheke for the purpose of speech recognition.
type ASRStream struct {
	PBStream diathekepb.Diatheke_StreamASRClient
}

// SendAudio to Diatheke for transcription. If the returned error
// is io.EOF, the stream has already been closed by the server and
// Result() should be called to get the final result.
func (asr *ASRStream) SendAudio(audio []byte) error {
	return asr.PBStream.Send(&diathekepb.ASRInput{
		Data: &diathekepb.ASRInput_Audio{
			Audio: audio,
		},
	})
}

// SendToken passes the given session token to Diatheke to update
// the speech recognition context. The session token must first be
// sent on the ASR stream before any audio will be recognized. If
// the stream  was created using client.NewSessionASRStream(), the
//first token was already sent.
//
// If the returned error is io.EOF, the Result() function should be
// called to get the final ASR result.
func (asr *ASRStream) SendToken(token *diathekepb.TokenData) error {
	return asr.PBStream.Send(&diathekepb.ASRInput{
		Data: &diathekepb.ASRInput_Token{
			Token: token,
		},
	})
}

// Result returns the result of speech recognition. This function
// may be called to end the audio stream early, which will force
// a transcription based on the audio received until this point,
// or in response to receiving an io.EOF error from PushAudio.
func (asr *ASRStream) Result() (*diathekepb.ASRResult, error) {
	return asr.PBStream.CloseAndRecv()
}

// ReadASRAudio is a convenience function to send audio from the
// given reader to the stream until a result is returned. If an
// error is encountered no result will be returned.
func ReadASRAudio(
	stream *ASRStream, reader io.Reader, buffSize int,
) (*diathekepb.ASRResult, error) {
	buffer := make([]byte, buffSize)
	for {
		// Pull audio data from the reader
		bytesRead, err := reader.Read(buffer)
		if err == io.EOF {
			// This indicates the reader has reached its end, in
			// which case we want to end the stream and get a result.
			break
		} else if err != nil {
			return nil, err
		}

		// Send the audio to the stream.
		err = stream.SendAudio(buffer[0:bytesRead])
		if err == io.EOF {
			// The server closed the stream, which means we
			// are ready to get a result.
			break
		} else if err != nil {
			return nil, err
		}
	}

	// Return the result
	return stream.Result()
}

// TTSStream represents a stream of audio data from Diatheke to the
// client containing synthesized speech.
type TTSStream struct {
	PBStream diathekepb.Diatheke_StreamTTSClient
}

// ReceiveAudio waits for the next chunk of audio data from the server.
// When the returned error is io.EOF, synthesis is complete and no more
// audio will be sent from Diatheke.
func (tts *TTSStream) ReceiveAudio() ([]byte, error) {
	reply, err := tts.PBStream.Recv()
	if err != nil {
		return nil, err
	}

	return reply.Audio, nil
}

// WriteTTSAudio is a convenience function to receive audio from the
// given TTSStream and send it to the writer until there is no more
// audio to receive.
func WriteTTSAudio(stream *TTSStream, writer io.Writer) error {
	for {
		// Wait for the next audio chunk
		audio, err := stream.ReceiveAudio()
		if err == io.EOF {
			return nil
		}
		if err != nil {
			return err
		}

		// Write the audio chunk
		if _, err = writer.Write(audio); err != nil {
			return err
		}
	}
}
