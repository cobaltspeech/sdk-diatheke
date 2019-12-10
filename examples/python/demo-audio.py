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
from textui import ToggleUI, print_red, print_green, print_blue
from audio_io import Recorder, Player


class SessionManager(object):
    """SessionManager coordinates interactions between the UI, the session,
    and the external audio applications (recording and playback).
    """

    def __init__(self, session, app_config):
        self.session = session
        self.verbose = app_config.verbose
        self.recording_config = app_config.recording
        self.playback_config = app_config.playback

        # Start the event stream on a separate thread
        self._event_thread = threading.Thread(target=self.handle_events)
        self._event_thread.start()

        # Start the tts reply stream on a separate thread
        self._tts_thread = threading.Thread(target=self.handle_tts_replies)
        self._tts_thread.start()

        # Set up variables to keep track of the audio state (whether things
        # are playing or recording).
        self.lock = threading.Lock()
        self._record_flag = False
        self._play_flag = False

        # Add a variable to keep track of the recording thread
        self._recording_thread = None

    def _set_recording_state(self, is_recording):
        with self.lock:
            self._record_flag = is_recording

    def _is_recording(self):
        with self.lock:
            if self._record_flag:
                return True
            else:
                return False

    def _set_playing_state(self, is_playing):
        with self.lock:
            self._play_flag = is_playing

    def _is_playing(self):
        with self.lock:
            if self._play_flag:
                return True
            else:
                return False

    def toggle_recording(self, on):
        """This is the callback function intended to be used with the ToggleUI.
        When a user presses the Enter key, this function is called, which either
        starts or stops the audio recording.
        """
        if on:
            self._set_recording_state(True)

            # Start the recording thread
            self._recording_thread = threading.Thread(target=self.record_audio)
            self._recording_thread.start()

        else:
            # Stop the recording thread by updating the recording state
            self._set_recording_state(False)

            # Wait for recording to finish
            self._recording_thread.join()

    def record_audio(self):
        """Records audio using the recording application and pushes the audio data
        to the session.
        """

        # Create the audio input stream for this session
        input_stream = self.session.stream_audio_input()

        # Start the recording application
        rec = Recorder(self.recording_config)
        rec.start()

        while self._is_recording():
            # Read audio data from the recording app
            audio_data = rec.read()

            if self._is_playing():
                # If playback is currently running, it is possible that the TTS audio
                # could be overheard in the recording. One (not very robust) way to
                # handle that issue is to mute recording during playback, which we
                # essentially do here by not forwarding the audio.
                continue

            # Send the audio to Diatheke
            input_stream.write(audio_data)

        # Stop the recording application
        rec.stop()

        # Let Diatheke know that no more audio will be coming on this stream.
        input_stream.finish()

    def handle_tts_replies(self):
        """Plays audio from the session using the external playback application.
        This function runs on its own thread.
        """

        # Create the stream
        tts_stream = session.stream_audio_replies()

        # Set up the playback application
        player = Player(self.playback_config)

        # Receive messages from the stream until it closes, which will happen
        # when the session ends
        for msg in tts_stream:
            # Handle each message based on its type
            if msg.HasField("text"):
                # This message contains the text of the reply, which comes
                # at the beginning of TTS. Start the playback app so it is
                # ready to play audio.
                player.start()
                self._set_playing_state(True)

            elif msg.HasField("data"):
                # This message contains audio data. We receive this message
                # until TTS synthesis is complete. Send the data to the
                # playback app. The data format is specified in the Diatheke
                # server configuration.
                player.push_audio(msg.data)

            elif msg.HasField("end"):
                # This message notifies us that TTS has finished. Stop
                # the playback application.
                player.stop()
                self._set_playing_state(False)

            else:
                print("Received unknown message in TTS replies from Diatheke.")

        print("- TTS reply stream closed")

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

        # Get the recognized text from the event
        text = "User: " + event.text

        # Color the text green if Diatheke recognized it, and red otherwise
        if event.valid_input:
            print_green(text)
        else:
            print_red(text)

    def handle_reply_event(event):
        """Handles reply events from Diatheke."""

        # Color the text blue to highlight that it is a reply
        print_blue("  Diatheke: " + event.text)

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
    banner = "\nDiatheke Audio Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n"
    banner += "Diatheke is Cobalt's dialog management engine."

    # Load the config file based on application args
    parser = argparse.ArgumentParser(description=banner)
    parser.add_argument('-config', '--config',
                        action='store', default='config.toml', type=str)
    args = parser.parse_args()
    cfg = config.load_file(args.config)

    # The following config params are required for this demo
    if not cfg.server.model_id:
        raise ValueError("Missing ModelID in the config file.")

    if not cfg.recording.application:
        raise ValueError("Missing Recording application in the config file.")

    if not cfg.playback.application:
        raise ValueError("Missing Playback application in the config file.")

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

    # Create our session manager to handle the coordination for this session
    # between all the various streams and external applications.
    manager = SessionManager(session, cfg)

    # Run the UI for this demo.
    try:
        ui = ToggleUI(off_prompt="(Press Enter to record, Ctrl+D to exit.)",
                      on_prompt="(Recording. Press Enter to stop.)",
                      toggle_callback=manager.toggle_recording)
        ui.run()

    except BaseException as err:
        print(err)

    finally:
        # Make sure we close the session when we are done.
        client.end_session(session_id)
        print("- Session ended")