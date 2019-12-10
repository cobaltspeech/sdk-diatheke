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

import toml


class ServerConfig(object):
    """Stores settings related to the server the Diatheke client
    should communicate with."""

    def __init__(self, server_dict):
        self.address = server_dict['Address']
        self.insecure = server_dict['Insecure']
        self.model_id = server_dict['ModelID']


class AudioConfig(object):
    """Stores settings related to the playback and recording apps
    that the demos use to handle audio IO."""

    def __init__(self, audio_dict):
        self.application = audio_dict['Application']
        self.args = audio_dict['Args']

    def arg_list(self):
        """Return the application's args as a list of strings"""

        return self.args.split()


class DemoConfig(object):
    """Stores settings for the Diatheke SDK demo applications"""

    def __init__(self, toml_dict):
        self.verbose = toml_dict['Verbose']
        self.server = ServerConfig(toml_dict['Server'])
        self.recording = AudioConfig(toml_dict['Recording'])
        self.playback = AudioConfig(toml_dict['Playback'])


def load_file(filename):
    """Loads the given config file (TOML format) and returns a new
    DemoConfig object."""

    toml_dict = toml.load(filename)
    cfg = DemoConfig(toml_dict)

    # Verify that the server address was specified (always required)
    if not cfg.server.address:
        raise ValueError("Missing server address in the config file.")

    return cfg
