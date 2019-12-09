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


class Session(object):
    """The session class is provided as a convenience for working with
    methods that require a session ID."""

    def __init__(self, session_id, client):
        """Create a new Session object for the given session_id and client."""
        self.id = session_id
        self.client = client

    def end_session(self):
        """Ends this session on the server."""
        self.client.end_session(self.id)

    def event_stream(self):
        """Returns a new event stream for this session."""
        return self.client.session_event_stream(self.id)

    def command_finished(self, command_status):
        """ Notifies Diatheke when a command has completed.

        The initial command request will come as part of a DiathekeEvent on the
        session's event stream. This method should always be called after
        receiving a command event when the command is completed (even if it is
        a long running command).
        """
        return self.client.command_finished(command_status)

    def stream_audio_input(self):
        """Begin an audio input stream for the given session. As the audio is
        recognized, Diatheke will respond with RecognizeEvents that include
        the transcription on the session's event stream. Transcriptions are not
        returned on this stream, but are automatically processed by Diatheke.

        Only one audio input stream per session should be running at a time to
        avoid confusing the speech recognition with multiple audio sources.
        """
        return self.client.stream_audio_input(self.id)

    def stream_audio_replies(self):
        """Create a stream to receive output audio from Diatheke specifically
        for the given session. The stream will use include start and end
        messages to indicate when a section of audio for a group of text
        begins and ends. Only one stream per session should be created.
        """
        return self.client.stream_audio_replies(self.id)

    def push_text(self, text):
        """Push text to Diatheke as part of a conversation for this session.

        This function may be used alone or in addition to an audio input
        stream to drive the conversation with Diatheke. On the server side,
        the audio input stream essentially calls this function automatically as
        transcriptions are available.
        """
        return self.client.push_text(self.id, text)
