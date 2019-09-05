package main

import (
	"context"
	"flag"
	"fmt"
	"io"
	"os"
	"strings"

	"github.com/c-bata/go-prompt"

	diatheke "github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
	"github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/diathekepb"
)

func main() {

	// Read the config file
	configFile := flag.String("config", "config.toml", "Path to the config file")
	flag.Parse()
	config, err := ReadConfigFile(*configFile)
	if err != nil {
		fmt.Printf("error reading config file: %v\n", err)
		os.Exit(1)
	}

	// Create the Diatheke client
	opts := []diatheke.Option{}
	if config.Server.Insecure {
		// NOTE: For production environments it is recommended to use a
		//       secure connection instead.
		opts = append(opts, diatheke.WithInsecure())
	}

	client, err := diatheke.NewClient(config.Server.Address, opts...)
	if err != nil {
		fmt.Printf("error creating diatheke client: %v\n", err)
		os.Exit(1)
	}

	// Run the main loop
	if err := Run(client, config); err != nil {
		fmt.Printf("error running the CLI: %v\n", err)
		// Don't exit here so that we can close the client.
	}

	// End the session and close the client
	client.Close()
}

// Run the CLI main loop. Mostly doing this in a separate
// function outside of main() so that I can better use defer
// and go routines without having to worry about os.Exit
// problems.
func Run(client *diatheke.Client, config Config) error {
	audioEnabled := config.UseAudio

	// Print the banner
	fmt.Printf("\nDiatheke CLI - Copyright (2019) Cobalt Speech and Language, Inc.\n\n")
	fmt.Printf("Diatheke is Cobalt's dialog management engine.\n")
	fmt.Printf("Enter commands at the prompt. To exit, use Ctl+D.\n")
	if audioEnabled {
		fmt.Printf("Audio is enabled. Use Ctl+A to start/stop recording audio.\n")
	}
	fmt.Printf("Type 'help' to see the CLI help.\n")
	fmt.Printf("\n\n")

	// Create a new session
	sessionID, err := client.NewSession(context.Background(), "1")
	if err != nil {
		return err
	}

	// Be sure to close the session when we are done
	defer func() {
		_, err := client.EndSession(context.Background(), sessionID)
		if err != nil {
			fmt.Printf("end session error: %v\n", err)
		}
	}()

	// Setup a context with a cancel func to close streams when we are
	// done with the loop
	cancelContext, cancel := context.WithCancel(context.Background())
	defer cancel()

	// Setup the command/notify handler
	stream, err := client.CommandAndNotify(cancelContext)
	if err != nil {
		return fmt.Errorf("failed to setup command handler: %v", err)
	}

	err = stream.Send(&diathekepb.CommandStatusUpdate{
		Update: &diathekepb.CommandStatusUpdate_SessionId{
			SessionId: sessionID,
		},
	})
	if err != nil {
		return fmt.Errorf("failed to setup command handler: %v", err)
	}

	go commandAndNotifyHandler(cancelContext, stream)

	// Setup the TTS/text reponse callack
	responseStream, err := client.SayCallback(cancelContext, sessionID)
	if err != nil {
		return fmt.Errorf("failed to setup response handler: %v", err)
	}
	go responseHandler(cancelContext, responseStream, config)

	// Setup a handler for text requests
	textHandler := func(raw string) {

		// Check if this is a help request
		trimmed := strings.TrimSpace(raw)
		lower := strings.ToLower(trimmed)
		if lower == "help" {
			fmt.Printf("Diatheke CLI Help -\n")
			fmt.Printf("  Enter commands at the prompt")
			if audioEnabled {
				fmt.Printf(" or record audio.")
			}

			fmt.Printf("\n\n")
			fmt.Printf("  Keyboard shortcuts:\n")
			fmt.Printf("  -------------------\n")

			if audioEnabled {
				fmt.Printf("  Ctl+A = start/stop recording audio\n")
			}

			fmt.Printf("  Ctl+D = exit\n\n")
		}

		if trimmed == "" {
			return
		}

		// Pass the text on to Diatheke
		request := diathekepb.PushTextRequest{
			SessionId: sessionID,
			Text:      trimmed,
		}
		client.PushText(context.Background(), &request)
	}

	// Setup autocomplete
	emptyAutoComplete := func(prompt.Document) []prompt.Suggest {
		return nil
	}

	// Setup options for the prompt
	options := []prompt.Option{
		prompt.OptionPrefix("Diatheke> "),
		prompt.OptionSwitchKeyBindMode(prompt.CommonKeyBind),
	}

	if audioEnabled {
		record := Recorder{
			ExeName: config.Recording.Application,
			ExeArgs: config.Recording.ArgList(),
		}
		isRecording := false

		audioToggle := func(buf *prompt.Buffer) {
			// Check whether we need to start or stop audio
			if isRecording {
				isRecording = false

				// Stop recording.
				record.Stop()
				fmt.Printf("\nRecording stopped.\n")
				return
			}

			// Start recording
			isRecording = true
			output, audioErr := record.Start()
			if audioErr != nil {
				fmt.Printf("\nerror starting recorder: %v\n", audioErr)
				return
			}

			go func() {
				audioErr = client.PushAudio(cancelContext, sessionID, output,
					func(result *diathekepb.TranscriptionResult) {
						fmt.Printf("\n(transcript) %s\n", result)
					})
				if audioErr != nil {
					fmt.Printf("\nerror pushing audio: %v\n", audioErr)
				}
			}()

			fmt.Printf("\nRecording. Press Ctl+A to stop.\n")
		}

		options = append(options, prompt.OptionAddKeyBind(
			prompt.KeyBind{
				Key: prompt.ControlA,
				Fn:  audioToggle,
			},
		))
	}

	// Create and run the prompt
	p := prompt.New(textHandler, emptyAutoComplete, options...)
	p.Run()

	return nil
}

func commandAndNotifyHandler(ctx context.Context,
	stream diathekepb.Diatheke_CommandAndNotifyClient) {
	for {
		if ctx.Err() != nil {
			return
		}

		command, err := stream.Recv()
		if err != nil {
			// Check if this error is because the context was cancelled
			select {
			case <-ctx.Done():
				// Do nothing
			default:
				fmt.Printf("\nerror receiving command data: %v\n", err)
			}

			return
		}

		fmt.Printf("\nrecieved command %v\n", command.CommandId)
		fmt.Printf("parameters: %v\n", command.Parameters)

		// TODO: Something awesome to execute the command

		// Update the diatheke state with a notification
		update := &diathekepb.CommandStatusUpdate{
			Update: &diathekepb.CommandStatusUpdate_Data_{
				Data: &diathekepb.CommandStatusUpdate_Data{
					CommandId:        command.CommandId,
					CommandStatus:    diathekepb.CommandStatusUpdate_Data_SUCCESS,
					StateData:        command.StateData,
					Parameters:       command.Parameters,
					ErrorMessageText: "",
				},
			},
		}

		if err := stream.Send(update); err != nil {
			fmt.Printf("\nerror sending command update: %v\n", err)
		}
	}
}

func responseHandler(ctx context.Context,
	stream diathekepb.Diatheke_SayCallbackClient,
	config Config) {

	for {
		if ctx.Err() != nil {
			return
		}

		response, err := stream.Recv()
		if err == io.EOF {
			// The stream was closed by the server
			return
		}

		if err != nil {
			// Check if this error is because the context was cancelled
			select {
			case <-ctx.Done():
				return
			default:
				fmt.Printf("\nerror receiving response: %v\n", err)
				return
			}
		}

		fmt.Printf("\nResponse: %s\n\n", response.Text)
		print()

		if config.UseAudio {
			Playback(response.Data, config.Playback)
		}
	}
}
