// Copyright (2021) Cobalt Speech and Language Inc.

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

// TranscribeStream represents a stream of data sent from the client
// to Diatheke for the purpose of transcribing speech.
type TranscribeStream struct {
	PBStream diathekepb.Diatheke_TranscribeClient
}

// SendAudio to Diatheke for transcription. If the returned error
// is io.EOF, the stream has already been closed by the server.
func (ts *TranscribeStream) SendAudio(audio []byte) error {
	return ts.PBStream.Send(&diathekepb.TranscribeInput{
		Data: &diathekepb.TranscribeInput_Audio{
			Audio: audio,
		},
	})
}

// SendAction passes the given transcribe action to Diatheke to use
// for speech recognition. The action must first be sent before
// any audio will be transcribed. If the stream was created using
// client.NewTranscribeStream(), the action was already sent.
func (ts *TranscribeStream) SendAction(action *diathekepb.TranscribeAction) error {
	return ts.PBStream.Send(&diathekepb.TranscribeInput{
		Data: &diathekepb.TranscribeInput_Action{
			Action: action,
		},
	})
}

// SendFinished informs the server that no more data will be
// sent over this stream. It is an error to call SendAudio()
// or SendAction() after calling this.
func (ts *TranscribeStream) SendFinished() error {
	return ts.PBStream.CloseSend()
}

// ReceiveResult waits for the next transcribe result from
// Diatheke. When the returned error is io.EOF, the stream has
// been closed and no more results will be sent from Diatheke.
func (ts *TranscribeStream) ReceiveResult() (*diathekepb.TranscribeResult, error) {
	return ts.PBStream.Recv()
}

type TranscribeResultHandler func(*diathekepb.TranscribeResult)

// ReadTranscribeAudio is a convenience function to send audio
// from the given reader to the stream. Results are processed
// by the given handler as they come. This function will block
// until the stream ends, either because the server closed it,
// or the reader returned io.EOF, or there was an error.
func ReadTranscribeAudio(
	stream *TranscribeStream,
	reader io.Reader,
	bytesBuffSize int,
	handler TranscribeResultHandler,
) error {
	// Read audio on a separate go routine
	sendErrCh := make(chan error)
	go func() {
		sendErrCh <- sendTranscribeAudio(stream, reader, bytesBuffSize)
	}()

	// In the meantime, handle results here
	var receiveErr error
	for {
		var result *diathekepb.TranscribeResult
		result, receiveErr = stream.ReceiveResult()
		if receiveErr != nil {
			break
		}

		handler(result)
	}

	// Get the error from the send side of the stream
	sendErr := <-sendErrCh

	if receiveErr == io.EOF {
		// This just indicates the stream closed. Return a
		// send error (if there was one).
		return sendErr
	}

	// Both of these are real errors, not io.EOF.
	if sendErr != nil && receiveErr != nil {
		return fmt.Errorf("send error: %v, receive error: %v",
			sendErr, receiveErr)
	}

	if sendErr != nil {
		return sendErr
	}

	return receiveErr
}

// helper function to handle the send side of the transcribe stream.
func sendTranscribeAudio(
	stream *TranscribeStream,
	reader io.Reader,
	bytesBuffSize int,
) error {
	buffer := make([]byte, bytesBuffSize)
	var err error
	var bytesRead int
	for {
		// Pull audio data from the reader
		bytesRead, err = reader.Read(buffer)
		if err != nil {
			break
		}

		// Send the audio to the stream.
		err = stream.SendAudio(buffer[0:bytesRead])
		if err != nil {
			break
		}
	}

	// Close the stream
	closeErr := stream.SendFinished()

	// This indicates that either the reader or the stream
	// reached its end. In either case, it is not an error,
	// so we return the stream close error (if there was one).
	if err == io.EOF {
		return closeErr
	}

	// If there was a close error too, return both errors.
	if closeErr != nil {
		return fmt.Errorf("%v, %v", err, closeErr)
	}

	// This indicates something went wrong with the reader or
	// the stream.
	return err
}
