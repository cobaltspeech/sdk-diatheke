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

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/config"
	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/textui"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
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

	// The model is required for this demo
	if cfg.Server.ModelID == "" {
		fatalErr = fmt.Errorf("missing Diatheke ModelID in the config file")
		return
	}

	printVerbose = cfg.Verbose

	// Print the banner
	fmt.Printf("\nDiatheke CLI Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n")
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

	handleEvents(session)

	// Setup the callback for the CLI. This will be called whenever the
	// user presses Enter.
	cb := func(userText string) {
		input := strings.TrimSpace(userText)
		if input == "" {
			// Ignore empty lines
			return
		}

		// Send the input to Diatheke
		if err := session.PushText(context.Background(), input); err != nil {
			textui.PrintError("Error: failed to push text to Diatheke: %v\n", err)
		}
	}

	// Create and run the CLI
	cli := textui.CLI{
		PromptText: "Diatheke> ",
		Callback:   cb,
	}

	cli.Run()
}

func handleEvents(session diatheke.Session) {
	// Create the event stream
	eventStream, err := session.EventStream(context.Background())
	if err != nil {
		textui.PrintError("Error creating event stream: %v\n", err)
		return
	}

	// Start a separate go routine to handle events coming from the server
	go func() {
		for {
			// Wait for the next event from the server
			event, err := eventStream.Recv()

			// This indicates the stream has finished, which will happen
			// when the session ends or the context used to create the stream
			// closes.
			if err == io.EOF {
				return
			}
			if err != nil {
				textui.PrintError("Event stream error: %v\n", err)
				return
			}

			// Check the event type
			switch e := event.Result.(type) {
			case *diathekepb.DiathekeEvent_Recognize:
				handleRecognizeEvent(e.Recognize)

			case *diathekepb.DiathekeEvent_Reply:
				handleReplyEvent(e.Reply)

			case *diathekepb.DiathekeEvent_Command:
				handleCommandEvent(e.Command, session)

			default:
				textui.PrintError("Error: received unknown event type from Diatheke\n")
			}
		}
	}()
}

// Handles Recognize events from Diatheke as they come.
func handleRecognizeEvent(event *diathekepb.RecognizeEvent) {
	// Alert the user when the input was not recognized. Here we don't
	// care so much about the text that was recognized because it will
	// be the same as what the user typed on the command line.
	if event.ValidInput {
		textui.PrintGreen("\nValid input\n")
	} else {
		textui.PrintRed("\nUnrecognized input\n")
	}
}

// Handles Reply events from Diatheke as they come.
func handleReplyEvent(event *diathekepb.ReplyEvent) {
	fmt.Printf("Reply: %v\n", event.Text)
}

// Handles Command events from Diatheke as they come.
func handleCommandEvent(event *diathekepb.CommandEvent, session diatheke.Session) {
	// Application specific code goes here

	if printVerbose {
		// This section demonstrates how to get parameters from the command
		// event. Set the Verbose field to true in the config file to
		// enable printing.
		fmt.Printf("    Command ID: %v\n", event.CommandId)
		fmt.Printf("    Parameters:\n")
		for param, value := range event.Parameters {
			fmt.Printf("      %v = %v\n", param, value)
		}
		fmt.Printf("\n")
	}

	// Setup the command status.
	status := &diathekepb.CommandStatus{
		SessionId: session.ID,
		CommandId: event.CommandId,

		// Indicate success or failure
		ReturnStatus: diathekepb.CommandStatus_SUCCESS,

		// Return parameters as necessary. Depending on the Diatheke model,
		// some commands may be expected to have output.
		OutputParameters: event.Parameters,

		// Internal data that should always be set from the original command
		// event.
		CommandStateId: event.CommandStateId,
	}

	// Notify Diatheke that the command has completed.
	if err := session.CommandFinished(context.Background(), status); err != nil {
		textui.PrintError("Error notifying Diatheke that command finished: %v\n", err)
	}
}
