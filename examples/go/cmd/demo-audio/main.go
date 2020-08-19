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
	"os"
	"sync"

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/config"
	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/textui"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
)

var printVerbose bool

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

	// The following are required for this demo
	if cfg.Server.ModelID == "" {
		fatalErr = fmt.Errorf("missing Diatheke ModelID in the config file")
		return
	}

	if cfg.Playback.Application == "" {
		fatalErr = fmt.Errorf("missing Playback application in the config file")
		return
	}

	if cfg.Recording.Application == "" {
		fatalErr = fmt.Errorf("missing Recording application in the config file")
		return
	}

	printVerbose = cfg.Verbose

	// Print the banner
	fmt.Printf("\nDiatheke Audio Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n")
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

	// Get a list of the available models
	models, err := client.ListModels(context.Background())
	if err != nil {
		fatalErr = fmt.Errorf("could not get Diatheke models: %v", err)
		return
	}

	// Display the list of models
	fmt.Printf("Available model IDs:\n")
	for _, m := range models {
		fmt.Printf("  %s\n", m)
	}
	fmt.Printf("\n")

	// Create a new session
	sessionID, err := client.NewSession(context.Background(), cfg.Server.ModelID)
	if err != nil {
		fatalErr = fmt.Errorf("failed to create session: %v", err)
		return
	}

	// Wrap the session in the convenience struct. As an alternative, you could
	// instead call all the methods from the client directly using the sessionID.
	session := diatheke.Session{
		ID:     sessionID,
		Parent: client,
	}
	defer session.EndSession(context.Background())

	// Create the session manager to handle the coordination for this session
	// between all the various streams and external applications.
	manager := sessionManager{
		Session:         session,
		RecordingConfig: cfg.Recording,
		PlaybackConfig:  cfg.Playback,
	}

	// Start the session's streams
	manager.StartEventStream()
	manager.StartAudioReplyStream()

	// With the session's streams set up, we are ready to start
	// the session itself.
	if err = session.Start(context.Background()); err != nil {
		fatalErr = fmt.Errorf("failed to start session: %v", err)
		return
	}

	// Run the CLI to toggle recording audio on and off
	textui.RunToggleCLI(&manager)
}

// SessionManager coordinates between the many streams and external
// applications used for a session.
type sessionManager struct {
	// The Session we are trying to manage.
	Session diatheke.Session

	// RecordingConfig will be used to start an external recording app.
	RecordingConfig config.AudioConfig

	// PlaybackConfig will be used to start an external playback app.
	PlaybackConfig config.AudioConfig

	// Internal data

	// Include a mutex and flags to coordinate between recording and
	// playback.
	mux sync.RWMutex

	// Flag used to start/stop the recording app.
	recording bool

	// Flag used to ignore ASR input during TTS output.
	playing bool
}

// GetPrompt implements the ToggleExecutor interface. It returns the
// correct prompts depending on what the given toggle state is.
func (manager *sessionManager) GetPrompt(toggleState bool) string {
	if toggleState {
		return "(Recording. Press Enter to stop.)"
	}

	return "(Press Enter to record, Ctrl+D to exit.)"
}

// ToggleOn implements the ToggleExecutor interface. This function
// starts the recording application.
func (manager *sessionManager) ToggleOn() {
	// Start recording audio. We use a bool shared across threads to notify
	// the go routine of when it should stop processing audio.
	manager.SetRecording(true)
	go manager.recordAudio()
}

// ToggleOff implements the ToggleExecutor interface. This function
// stops the recording application.
func (manager *sessionManager) ToggleOff() {
	// Stop recording audio by setting our recording flag to false.
	manager.SetRecording(false)
}

// StartEventStream creates a new event stream for the session being
// managed.
func (manager *sessionManager) StartEventStream() {
	// Start the event stream on a separate go routine
	go manager.handleEvents()
}

// StartAudioReply stream creates a new TTS reply stream for the session
// being managed.
func (manager *sessionManager) StartAudioReplyStream() {
	// Start the the TTS stream for the session on a separate go routine
	go manager.handleTTSReplies()
}
