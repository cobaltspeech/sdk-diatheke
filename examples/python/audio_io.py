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

import subprocess


class Recorder(object):
    """Recorder launches an external application to handle recording audio."""

    def __init__(self, audio_config, bufsize=8096):
        self.args = [audio_config.application]
        self.args.extend(audio_config.arg_list())
        self.bufsize = bufsize
        self.process = None

    def start(self):
        """Start the external recording application."""

        # Ignore if we already started it
        if self.process is not None:
            return

        # Start the subprocess
        self.process = subprocess.Popen(args=self.args,
                                        bufsize=self.bufsize,
                                        stdout=subprocess.PIPE)

    def stop(self):
        """Stop the external recording application."""

        # Ignore if it is not running
        if self.process is None:
            return

        # Stop the subprocess
        self.process.stdout.close()
        self.process.terminate()
        self.process = None

    def read(self):
        """Read audio data from the external recording application."""

        # Raise an error if we haven't started the app
        if self.process is None:
            raise RuntimeError("Recording application is not running")

        # Get the data from stdout
        return self.process.stdout.read(self.bufsize)


class Player(object):
    """Player launches an external application to handle audio playback."""

    def __init__(self, audio_config):
        self.args = [audio_config.application]
        self.args.extend(audio_config.arg_list())
        self.process = None

    def start(self):
        """Start the external playback application."""

        # Ignore if we already started it
        if self.process is not None:
            return

        # Start the subprocess
        self.process = subprocess.Popen(args=self.args,
                                        stdin=subprocess.PIPE)

    def stop(self):
        """Stop the external playback application."""

        # Ignore if it is not running
        if self.process is None:
            return

        # Stop the subprocess
        self.process.stdin.close()
        self.process.wait()
        self.process = None

    def push_audio(self, audio):
        """Push audio data to the external playback application. start()
        should be called prior to using this method. Audio should be binary
        data."""

        # Raise an error if we haven't started the app
        if self.process is None:
            raise RuntimeError("Playback application is not running")

        # Write the audio data to the external application's stdin.
        self.process.stdin.write(audio)
