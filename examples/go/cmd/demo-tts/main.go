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
	"strings"

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

	// The playback app is required for this demo
	if cfg.Playback.Application == "" {
		fatalErr = fmt.Errorf("missing playback application in config file")
		return
	}

	// Print the banner
	fmt.Printf("\nDiatheke TTS Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n")
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

	// Prompt the user for the Luna model
	modelID := textui.SimplePrompt("Please enter the Luna voice model ID: ")

	// Setup our player
	player := TTSPlayer{
		Client:       client,
		ModelID:      modelID,
		PlayerConfig: cfg.Playback,
	}

	// Setup the callback for the CLI. This will be called whenever the
	// user presses Enter.
	cb := func(userText string) {
		input := strings.TrimSpace(userText)
		if input == "" {
			// Ignore empty lines
			return
		}

		// Run streaming TTS with the client
		ttsErr := player.StreamTTS(input)
		if ttsErr != nil {
			textui.PrintError("Streaming TTS error: %v\n", ttsErr)
		}
	}

	// Create and run the CLI
	cli := textui.CLI{
		PromptText: "Diatheke TTS> ",
		Callback:   cb,
	}

	cli.Run()
}

// TTSPlayer is a convenience struct to help keep this demo organized.
// It manages getting a TTS stream from Diatheke and playing the audio
// on the external playback application.
type TTSPlayer struct {
	Client       *diatheke.Client
	ModelID      string
	PlayerConfig config.AudioConfig
}

// StreamTTS demonstrates how to create a TTS stream using the Diatheke
// Client.
func (tts *TTSPlayer) StreamTTS(text string) error {
	fmt.Printf("Synthesizing using voice model %s ...\n", tts.ModelID)

	// Start the player application
	player := audio.NewPlayer(tts.PlayerConfig)
	if err := player.Start(); err != nil {
		return err
	}

	// Make sure the player is stopped when we finish
	defer player.Stop()

	// Create the TTS stream for the given text. We will create a
	// cancellable context so that we can kill the stream early if
	// necessary.
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	stream, err := tts.Client.StreamTTS(ctx, tts.ModelID, text)
	if err != nil {
		return err
	}

	// Receive data from the TTS stream until it is closed, which will
	// happen when synthesis is complete, or the context used to create
	// the stream is cancelled.
	for {
		// Wait for data from the server
		msg, err := stream.Recv()
		if err == io.EOF {
			// The stream finished
			break
		}
		if err != nil {
			return err
		}

		// Push the audio data to our playback app
		if err := player.PushAudio(msg.Data); err != nil {
			return err
		}
	}

	fmt.Printf("Synthesis complete.\n\n")
	return nil
}
