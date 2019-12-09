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

import grpc

from diatheke_pb2_grpc import DiathekeStub
from diatheke_pb2 import Empty, NewSessionRequest, SessionID, PushTextRequest, TTSRequest
from streams import AudioInputStream, ASRStream


class Client(object):
    def __init__(self, server_address, insecure=False,
                 server_certificate=None,
                 client_certificate=None,
                 client_key=None):
        """  Creates a new Diatheke Client object.
        Args:
            server_address: host:port of where Diatheke server is running (string)
            insecure: If set to true, an insecure grpc channel is used.
                      Otherwise, a channel with transport security is used.
            server_certificate:  PEM certificate as byte string which is used as a  
                                 root certificate that can validate the certificate 
                                 presented by the server we are connecting to. Use this 
                                 when connecting to an instance of cubic server that is 
                                 using a self-signed certificate.
            client_certificate:  PEM certificate as bytes string presented by this Client when
                                 connecting to a server. Use this when setting up mutually 
                                 authenticated TLS. The clientKey must also be provided.
            client_key:  PEM key as byte string presented by this Client when
                         connecting to a server. Use this when setting up mutually 
                         authenticated TLS. The clientCertificate must also be provided.
        """
        self.server_address = server_address
        self.insecure = insecure

        if insecure:
            # no transport layer security (TLS)
            self._channel = grpc.insecure_channel(server_address)
        else:
            # using a TLS endpoint with optional certificates for mutual authentication
            if client_certificate is not None and client_key is None:
                raise ValueError("client key must also be provided")
            if client_key is not None and client_certificate is None:
                raise ValueError("client certificate must also be provided")
            self._creds = grpc.ssl_channel_credentials(
                root_certificates=server_certificate,
                private_key=client_key,
                certificate_chain=client_certificate)
            self._channel = grpc.secure_channel(server_address, self._creds)

        self._client = DiathekeStub(self._channel)

    def diatheke_version(self):
        """Returns the version of the connected server."""
        return self._client.Version(Empty()).server

    def list_models(self):
        """Lists the models available to the Diatheke server, as specified in
        the server's config file."""
        return self._client.Models(Empty()).models

    def new_session(self, model):
        """Creates a new session and returns its ID."""
        return self._client.NewSession(NewSessionRequest(model=model)).session_id

    def end_session(self, session_id):
        """Ends the specified session."""
        self._client.EndSession(SessionID(session_id=session_id))

    def session_event_stream(self, session_id):
        """Returns a new event stream for the given session."""
        return self._client.SessionEventStream(SessionID(session_id=session_id))

    def command_finished(self, command_status):
        """ Notifies Diatheke when a command has completed.

        The initial command request will come as part of a DiathekeEvent on the
        session's event stream. This method should always be called after
        receiving a command event when the command is completed (even if it is
        a long running command).
        """
        self._client.CommandFinished(command_status)

    def stream_audio_input(self, session_id):
        """Begin an audio input stream for the given session. As the audio is
        recognized, Diatheke will respond with RecognizeEvents that include
        the transcription on the session's event stream. Transcriptions are not
        returned on this stream, but are automatically processed by Diatheke.

        Only one audio input stream per session should be running at a time to
        avoid confusing the speech recognition with multiple audio sources.
        """
        return AudioInputStream(self._client, session_id)

    def stream_audio_replies(self, session_id):
        """Create a stream to receive output audio from Diatheke specifically
        for the given session. The stream will use include start and end
        messages to indicate when a section of audio for a group of text
        begins and ends. Only one stream per session should be created.
        """
        return self._client.StreamAudioReplies(SessionID(session_id=session_id))

    def push_text(self, session_id, text):
        """Push text to Diatheke as part of a conversation for the given session.

        This function may be used alone or in addition to an audio input
        stream to drive the conversation with Diatheke. On the server side,
        the audio input stream essentially calls this function automatically as
        transcriptions are available.
        """
        request = PushTextRequest(session_id=session_id,
                                  text=text)
        self._client.PushText(request)

    def stream_asr(self, model):
        """Creates a speech recognition stream unrelated to a session using the
        specified Cubic model.

        This will create a bidirectional stream. The client sends audio to the
        server and receives transcriptions as they become available from the
        receiving half of the stream.
        """
        return ASRStream(self._client, model)

    def stream_tts(self, model, text):
        """Creates a text to speech synthesis stream unrelated to a session using
        the specified Luna model and text to synthesize.

        Synthesized audio is returned by the stream until synthesis is complete,
        at which point the stream will close.
        """
        request = TTSRequest(model=model,
                             text=text)
        return self._client.StreamTTS(request)
