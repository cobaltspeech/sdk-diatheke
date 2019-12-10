#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright(2019) Cobalt Speech and Language Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http: // www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import threading
import diatheke

import config
from textui import PromptUI, print_red, print_green


class SessionManager(object):
    """SessionManager coordinates interactions between the UI and the session."""
    def __init__(self, session, print_verbose):
        self.session = session
        self.verbose = print_verbose

        # Start the event stream on a separate thread
        self._event_thread = threading.Thread(target=self.handle_events)
        self._event_thread.start()

    def handle_prompt(self, text):
        """This is the callback function intended to be used with the PromptUI.
        When the user submits text from the prompt, this function pushes the
        text to Diatheke.
        """
        if not text:
            # Ignore if there is no text
            return

        # Send the text to Diatheke
        self.session.push_text(text)

    def handle_events(self):
        """Handles events as they come from Diatheke. This function runs on
        its own thread.
        """
        event_stream = self.session.event_stream()

        # Receive events from the stream until it closes, which will happen
        # when the session ends.
        for event in event_stream:
            # Handle each event based on its type
            if event.HasField("recognize"):
                SessionManager.handle_recognize_event(event.recognize)

            elif event.HasField("reply"):
                SessionManager.handle_reply_event(event.reply)

            elif event.HasField("command"):
                self.handle_command_event(event.command)

            else:
                print("Received unknown event type from Diatheke.")

        print("- Event stream closed")

    def handle_recognize_event(event):
        """Handles RecognizeEvents from Diatheke."""

        # Alert the user when the input was not recognized. Here we don't
        # care so much about the text that was recognized because it will
        # be the same as what the user typed on the command line.

        # Color the text green if Diatheke recognized it, and red otherwise
        if event.valid_input:
            print_green("\nValid input")
        else:
            print_red("\nUnrecognized input")

    def handle_reply_event(event):
        """Handles reply events from Diatheke."""
        print("Reply: " + event.text)

    def handle_command_event(self, event):
        """Handles CommandEvents from Diatheke."""

        # Application specific code goes here.
        if self.verbose:
            # This section demonstrates how to get parameters from the command
            # event. Set the Verbose field to true in the config file to
            # enable printing.
            print("    Command ID: " + event.command_id)
            print("    Parameters:")
            for param in event.parameters:
                value = event.parameters[param]
                print("      {} = {}".format(param, value))
            print("\n")

        # Set up the command status to return to Diatheke
        result = diatheke.CommandStatus()
        result.session_id = self.session.id
        result.command_id = event.command_id
        result.return_status = diatheke.CommandStatus.SUCCESS
        result.output_parameters.update(event.parameters)
        result.command_state_id = event.command_state_id

        # Notify Diatheke that the command has completed.
        self.session.command_finished(result)


if __name__ == "__main__":
    banner = "\nDiatheke CLI Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n"
    banner += "Diatheke is Cobalt's dialog management engine."

    # Load the config file based on application args
    parser = argparse.ArgumentParser(description=banner)
    parser.add_argument('-config', '--config',
                        action='store', default='config.toml', type=str)
    args = parser.parse_args()
    cfg = config.load_file(args.config)

    # The model is required for this demo
    if not cfg.server.model_id:
        raise ValueError("Missing ModelID in the config file.")

    # Print the banner
    print(banner)

    # Create the Diatheke client
    client = diatheke.Client(cfg.server.address, cfg.server.insecure)

    # Print the Diatheke version and server info
    print("Diatheke version {}".format(client.diatheke_version()))
    print("Connected to {}\n".format(client.server_address))

    # Print the list of available models
    print("Available models:")
    models = client.list_models()
    for m in models:
        print("  {}".format(m))
    print("\n")

    # Create a new session
    session_id = client.new_session(cfg.server.model_id)

    # Wrap the session in the convenience class. As an alternative, you could
    # instead call methods directly from the client using the session ID.
    session = diatheke.Session(session_id, client)

    # Create our session manager to handle sending text to Diatheke and receiving
    # events in response.
    manager = SessionManager(session, cfg.verbose)

    # Run the UI for this demo.
    try:
        ui = PromptUI(prompt="Diatheke> ",
                      text_callback=manager.handle_prompt)
        ui.run()

    except BaseException as err:
        print(err)

    finally:
        # Make sure we close the session when we are done.
        client.end_session(session_id)
        print("- Session ended")
