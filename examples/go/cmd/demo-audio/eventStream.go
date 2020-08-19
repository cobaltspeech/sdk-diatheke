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
	"fmt"
	"io"

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/textui"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
)

// HandleEvents demonstrates how to use a session Event stream.
func (manager *sessionManager) handleEvents() {
	// Create the stream
	eventStream, err := manager.Session.EventStream(context.Background())
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
				handleCommandEvent(e.Command, manager.Session)

			default:
				textui.PrintError("Error: received unknown event type from Diatheke\n")
			}
		}
	}()
}

// Handles Recognize events from Diatheke.
func handleRecognizeEvent(event *diathekepb.RecognizeEvent) {
	// Color the text green if Diatheke recognized it, and red otherwise
	if event.ValidInput {
		textui.PrintGreen("User: %s\n", event.Text)
	} else {
		textui.PrintRed("User: %s\n", event.Text)
	}
}

// Handles Reply events from Diatheke.
func handleReplyEvent(event *diathekepb.ReplyEvent) {
	// Color the text blue to highlight that it is a reply.
	textui.PrintBlue("  Diatheke: %s\n\n", event.Text)
}

// Handles Command events from Diatheke.
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
