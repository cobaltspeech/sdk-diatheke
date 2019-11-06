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
	"flag"
	"fmt"
	"io"
	"os"
	"sync"

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/audio"

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/config"
	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/textui"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
)

func main() {
	// Setup a deferred function to handle exiting in response to a
	// fatal error, which will call os.Exit. We do this because calling
	// os.Exit does not allow deferred functions to be called, and there
	// are some we use later that we want to ensure are called.
	var fatalErr error
	defer func() {
		if fatalErr != nil {
			textui.PrintError("Error: %v\n", fatalErr)
			os.Exit(1)
		}

		fmt.Printf("Exiting...\n")
	}()

	// Read the config file
	configFile := flag.String("config", "config.toml", "Path to the config file")
	flag.Parse()
	cfg, err := config.ReadConfigFile(*configFile)
	if err != nil {
		fatalErr = fmt.Errorf("error reading config file: %v", err)
		return
	}

	// The recording app is required for this demo
	if cfg.Recording.Application == "" {
		fatalErr = fmt.Errorf("missing recording application in config file")
		return
	}

	// Print the banner
	fmt.Printf("\nDiatheke ASR Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n")
	fmt.Printf("Diatheke is Cobalt's dialog management engine.\n")

	// Create the Diatheke client
	opts := []diatheke.Option{}
	if cfg.Server.Insecure {
		// NOTE: Secure connections are recommended for production
		opts = append(opts, diatheke.WithInsecure())
	}

	client, err := diatheke.NewClient(cfg.Server.Address, opts...)
	if err != nil {
		fatalErr = fmt.Errorf("error creating diatheke client: %v", err)
		return
	}

	// Make sure the client is closed when we exit
	defer client.Close()

	// Display the diatheke version
	version, err := client.DiathekeVersion(context.Background())
	if err != nil {
		fatalErr = fmt.Errorf("could not get Diatheke version: %v", err)
		return
	}
	fmt.Printf("Diatheke version: %v\n", version)
	fmt.Printf("Connected to %v\n\n", cfg.Server.Address)

	// Prompt the user for the Cubic model
	modelID := textui.SimplePrompt("Please enter the Cubic model ID: ")

	// Create the ASR manager to handle coordination between the
	// bidirectional stream and recording application.
	manager := asrManager{
		Client:          client,
		ModelID:         modelID,
		RecordingConfig: cfg.Recording,
	}

	// Run the CLI to toggle recording audio on and off.
	textui.RunToggleCLI(&manager)
}

// asrManager implements the ToggleExecutor interface and handles
// coordinating the bidirectional ASR stream and recording application.
type asrManager struct {
	// Client will be used to create the ASR stream.
	Client *diatheke.Client

	// ModelID defines the Cubic model we will use for the ASR stream.
	ModelID string

	// RecordingConfig will be used to start an external recording
	// application.
	RecordingConfig config.AudioConfig

	// Include a mutex and flag to control the recording app
	mux       sync.RWMutex
	recording bool
}

// GetPrompt implements the ToggleExecutor interface. It returns the
// correct prompts depending on what the given toggle state is.
func (manager *asrManager) GetPrompt(toggleState bool) string {
	if toggleState {
		return "(Recording)"
	}

	return "(Press Enter to record, Ctrl+D to exit)"
}

// ToggleOn implements the ToggleExecutor interface. This function
// starts the recording application.
func (manager *asrManager) ToggleOn() {
	// Set the recording flag that we will use to terminate the
	// recording app later.
	manager.SetRecording(true)

	// Start the ASR stream
	manager.streamASR()
}

// ToggleOff implements the ToggleExecutor interface. This function
// stops the recording application.
func (manager *asrManager) ToggleOff() {
	// Set the recording flag, which will terminate the recording app.
	manager.SetRecording(false)
}

// StreamASR demonstrates how to setup the bi-directional ASR stream.
func (manager *asrManager) streamASR() {
	// Create the bidirectional stream. It is safe to call the read
	// and write portions of this stream concurrently.
	stream, err := manager.Client.StreamASR(context.Background(), manager.ModelID)
	if err != nil {
		textui.PrintError("Error creating ASR stream: %v\n", err)
		return
	}

	fmt.Printf("Recognizing using Cubic model %s ...\n", manager.ModelID)

	// Push audio on one go routine until the user toggles recording off
	go manager.recordAudio(stream)

	// Receive transcripts on another go routine.
	go manager.handleResults(stream)
}

// RecordAudio handles pushing audio data from the external recording
// app to the given ASR stream.
func (manager *asrManager) recordAudio(stream *diatheke.ASRStream) {
	// Make sure we alert Diatheke that no more audio will be coming
	// from the client after we exit this function.
	defer stream.AudioFinished()

	// Start the recording application
	rec := audio.NewRecorder(manager.RecordingConfig)
	if err := rec.Start(); err != nil {
		textui.PrintError("Failed to start recording app: %v\n", err)
		return
	}

	// Make sure we stop the recording app when we exit
	defer rec.Stop()

	// Create an 8kB buffer to hold the audio
	buffer := make([]byte, 8192)

	// Send the recorded audio data to Diatheke until the user toggles
	// the recorder off
	for manager.IsRecording() {

		// Read the audio
		bytesRead, err := rec.Read(buffer)
		if err != nil {
			textui.PrintError("error reading audio data: %v\n", err)
			continue
		}

		// Send the audio to Diatheke
		if _, err := stream.Write(buffer[0:bytesRead]); err != nil {
			textui.PrintError("error pushing audio data: %v\n", err)
		}
	}
}

// HandleResults demonstrates how to receive transcripts from the
// given ASR stream.
func (manager *asrManager) handleResults(stream *diatheke.ASRStream) {
	for {
		// Wait for a response from the server.
		response, err := stream.Receive()
		if err == io.EOF {
			// The stream finished. This will happen after the stream's
			// AudioFinished() method is called at the end of recording.
			return
		}
		if err != nil {
			textui.PrintError("error receiving transcript: %v\n", err)
			return
		}

		// Display the transcript
		fmt.Printf("ASR Response:\n")
		fmt.Printf("  Transcription: %s\n", response.Text)
		fmt.Printf("  Confidence Score: %v\n\n", response.ConfidenceScore)
	}
}

// SetRecording sets the audio state for recording.
func (manager *asrManager) SetRecording(on bool) {
	manager.mux.Lock()
	defer manager.mux.Unlock()
	manager.recording = on
}

// IsRecording returns true if audio is being recorded.
func (manager *asrManager) IsRecording() bool {
	manager.mux.RLock()
	defer manager.mux.RUnlock()
	return manager.recording
}
