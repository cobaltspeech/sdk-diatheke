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
import diatheke

import config
from textui import PromptUI
from audio_io import Player


class TTSPlayer(object):
    """TTSPlayer coordinates interaction between the UI and the Diatheke client."""

    def __init__(self, client, model_id, playback_config):
        self.client = client
        self.model_id = model_id
        self.playback_config = playback_config

    def run_tts(self, text):
        """This is the callback function intended to be used with the PromptUI.
        When the user submits text from the prompt, this function sends the
        text to Diatheke for voice synthesis.
        """

        print("Synthesizing using voice model {} ...".format(self.model_id))

        # Start the player application
        player = Player(self.playback_config)
        player.start()

        # Create the TTS stream for the supplied text.
        tts_stream = self.client.stream_tts(self.model_id, text)

        # Receive data from the TTS stream until it is closed, which will
        # happen when synthesis is complete.
        for response in tts_stream:
            player.push_audio(response.data)

        # Stop the player application
        player.stop()
        print("Synthesis complete.")


if __name__ == "__main__":
    banner = "\nDiatheke TTS Demo - Copyright (2019) Cobalt Speech and Language, Inc.\n"
    banner += "Diatheke is Cobalt's dialog management engine."

    # Load the config file based on application args
    parser = argparse.ArgumentParser(description=banner)
    parser.add_argument('-config', '--config',
                        action='store', default='config.toml', type=str)
    args = parser.parse_args()
    cfg = config.load_file(args.config)

    # The playback app is required for this demo
    if not cfg.playback.application:
        raise ValueError("Missing Playback application in the config file.")

    # Print the banner
    print(banner)

    # Create the Diatheke client
    client = diatheke.Client(cfg.server.address, cfg.server.insecure)

    # Print the Diatheke version and server info
    print("Diatheke version {}".format(client.diatheke_version()))
    print("Connected to {}\n".format(client.server_address))

    # Prompt the user for a Luna model
    luna_model_id = input("Please enter the Luna model ID: ")
    print("\n")

    # Create an instance of our TTSPlayer to handle running the TTS stream
    player = TTSPlayer(client, luna_model_id, cfg.playback)

    # Run the UI for this demo.
    try:
        ui = PromptUI(prompt="Diatheke TTS> ",
                      text_callback=player.run_tts)
        ui.run()

    except BaseException as err:
        print(err)
