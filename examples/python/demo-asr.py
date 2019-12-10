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
from textui import ToggleUI
from audio_io import Recorder


class ASRManager(object):
    """The ASRManager coordinates interactions between the Diatheke client,
    the audio recording application, and the UI."""

    def __init__(self, client, model_id, recording_config):
        self.client = client
        self.model_id = model_id
        self.recording_config = recording_config

        # Set up variables to keep track of the audio state (whether we are
        # recording or not).
        self.lock = threading.Lock()
        self._record_flag = False

        # Create variables to hold the recording and result handling threads
        self._recording_thread = None
        self._results_thread = None

    def _set_recording(self, is_recording):
        with self.lock:
            self._record_flag = is_recording

    def _is_recording(self):
        with self.lock:
            if self._record_flag:
                return True
            else:
                return False

    def toggle_recording(self, on):
        """This is the callback function intended to be used with the ToggleUI.
        When a user presses the Enter key, this function is called, which either
        starts or stops the audio recording.
        """
        if on:
            # Set the recording flag that will be used later to stop the
            # recording thread.
            self._set_recording(True)

            self.run_stream()

        else:
            # Stop the recording thread by updating the recording state
            self._set_recording(False)

            # Wait for the recording to finish
            self._recording_thread.join()
            self._results_thread.join()

    def run_stream(self):
        """Starts the ASR stream. Audio recording is done on one thread while
        returned transcriptions are handled on a separate thread.
        """

        # Create the bidirectional stream. It is safe to call the read and
        # write portions of the stream concurrently.
        asr_stream = self.client.stream_asr(self.model_id)

        # Start the recording thread.
        self._recording_thread = threading.Thread(target=self.record_audio,
                                                  args=(asr_stream,))
        self._recording_thread.start()

        # Receive transcripts on another thread.
        self._results_thread = threading.Thread(target=ASRManager.handle_results,
                                                args=(asr_stream,))
        self._results_thread.start()

    def record_audio(self, asr_stream):
        """Records audio using the recording application and pushes the audio data
        to the ASR stream.
        """

        # Start the recording application
        rec = Recorder(self.recording_config)
        rec.start()

        while self._is_recording():
            # Read audio data from the recording app
            audio_data = rec.read()

            # Send the audio to Diatheke
            asr_stream.write(audio_data)

        # Stop the recording application
        rec.stop()

        # Let Diatheke know that no more audio will be coming on this stream
        asr_stream.audio_finished()

    def handle_results(asr_stream):
        """Processes transcriptions as they are returned from the server on the
        ASR stream."""

        # Wait for results until the stream is closed, which will happen after
        # the audio_finished() method has been called on the stream (i.e., when
        # recording is done).
        for result in asr_stream.result_stream:
            # Display the transcript
            print("ASR Response:")
            print("  Transcription: " + result.text)
            print("  Confidence Score: {}\n".format(result.confidence_score))


if __name__ == "__main__":
    banner = "\nDiatheke ASR Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n"
    banner += "Diatheke is Cobalt's dialog management engine."

    # Load the config file based on application args
    parser = argparse.ArgumentParser(description=banner)
    parser.add_argument('-config', '--config',
                        action='store', default='config.toml', type=str)
    args = parser.parse_args()
    cfg = config.load_file(args.config)

    # The recording app is required for this demo
    if not cfg.recording.application:
        raise ValueError("Missing Recording application in the config file.")

    # Print the banner
    print(banner)

    # Create the Diatheke client
    client = diatheke.Client(cfg.server.address, cfg.server.insecure)

    # Print the Diatheke version and server info
    print("Diatheke version {}".format(client.diatheke_version()))
    print("Connected to {}\n".format(client.server_address))

    # Prompt the user for a Cubic model
    cubic_model_id = input("Please enter the Cubic model ID: ")
    print("\n")

    # Create an instance of the ASR manager to handle coordinating
    # between the bidirectional ASR stream and recording application.
    manager = ASRManager(client, cubic_model_id, cfg.recording)

    # Run the UI for this demo.
    try:
        ui = ToggleUI(off_prompt="(Press Enter to record, Ctrl+D to exit.)",
                      on_prompt="(Recording. Press Enter to stop.)",
                      toggle_callback=manager.toggle_recording)
        ui.run()

    except BaseException as err:
        print(err)
