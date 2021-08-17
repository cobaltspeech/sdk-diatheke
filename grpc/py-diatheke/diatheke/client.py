# -*- coding: utf-8 -*-
#
# Copyright(2021) Cobalt Speech and Language Inc.
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
import io

from diatheke_pb2_grpc import DiathekeStub
from diatheke_pb2 import Empty, SessionStart, TokenData, SessionInput, TextInput, SetStory, TranscribeInput, ASRInput
from streams import ASRStream, TranscribeStream


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

    def __del__(self):
        """ Closes and cleans up the client. """
        try:
            self._channel.close()
        except AttributeError:
            # client wasn't fully instantiated, no channel to close
            pass

    def version(self):
        """Returns the version information of the connected server."""
        return self._client.Version(Empty())

    def list_models(self):
        """Lists the models available to the Diatheke server, as specified in
        the server's config file."""
        return self._client.ListModels(Empty()).models

    def create_session(self, model):
        """Creates a new session using the specified model ID and return
        the session token and actions."""
        return self._client.CreateSession(SessionStart(model_id=model))

    def create_session_with_wakeword(self, model, wakeword):
        """Creates a new session using the specified model ID and custom
        wakeword. The wakeword will only have an effect if the model has
        wakeword detection enabled."""
        return self._client.CreateSession(SessionStart(model_id=model,
                                                       wakeword=wakeword))

    def delete_session(self, token):
        """Cleans up the given token. Behavior is undefined if the given
        token is used again after calling this function."""
        self._client.DeleteSession(token)

    def process_text(self, token, text):
        """Sends the given text to Diatheke and returns an updated session
        token."""
        req = SessionInput(token=token, text=TextInput(text=text))
        return self._client.UpdateSession(req)

    def process_asr_result(self, token, result):
        """Sends the given ASR result to Diatheke and returns an updated
        session token."""
        req = SessionInput(token=token, asr=result)
        return self._client.UpdateSession(req)

    def process_command_result(self, token, result):
        """Sends the given command result to Diatheke and returns an updated
        session token. This function should be called in response to a command
        action Diatheke sent previously."""
        req = SessionInput(token=token, cmd=result)
        return self._client.UpdateSession(req)

    def set_story(self, token, story_id, params):
        """Changes the current story for a Diatheke session. Returns an
        updated session token."""
        story = SetStory(story_id=story_id, parameters=params)
        req = SessionInput(token=token, story=story)
        return self._client.UpdateSession(req)

    def new_session_asr_stream(self, token):
        """Creates a new stream to transcribe audio for the given
        session token."""
        # Create the ASR stream object and send the session token
        stream = ASRStream(client_stub=self._client)
        stream.send_token(token)
        return stream

    def new_tts_stream(self, reply):
        """Creates a new stream to receive TTS audio from Diatheke based
        on the given reply action."""
        return self._client.StreamTTS(reply)

    def new_transcribe_stream(self, action):
        """Creates a new stream for transcriptions usually in response to
        a transcribe action in the session output. This stream differs from
        an ASR stream in purpose - where a session's ASR stream accepts audio
        to continue a conversation with the system, a transcribe stream
        accepts audio for the sole purpose of getting a transcription that
        the client can use for any purpose (e.g., note taking, send a message,
        etc.)."""
        stream = TranscribeStream(client_stub=self._client)
        stream.send_action(action)
        return stream

    def read_asr_audio(self, token, reader, buff_size):
        """Convenience function to create an ASR stream and send audio
        from the given reader to the stream. This function blocks until
        a result is returned. Data is sent in chunks defined by buff_size."""
        # Check if we have a text or byte reader
        is_text = isinstance(reader, io.TextIOBase)

        # Set up a generator function to send the session token and audio
        # data to Diatheke.
        def send_data():
            yield ASRInput(token=token)
            while True:
                data = reader.read(buff_size)
                if (is_text and data == '') or (not is_text and data == b''):
                    # Reached EOF
                    return
                
                # Send the audio
                yield ASRInput(audio=data)
        
        # Run the stream
        return self._client.StreamASR(send_data())

    def write_tts_audio(self, reply_action, writer):
        """Convenience function to create a TTS stream and send the audio
        to the given writer. This function blocks until there is no more
        audio to receive."""
        # Check if we have a text or byte writer
        is_text = isinstance(writer, io.TextIOBase)

        # Create the stream
        stream = self.new_tts_stream(reply_action)
        for data in stream:
            if is_text:
                # Convert the text to a string before writing
                writer.write(str(data.audio))
            else:
                writer.write(data.audio)

    def read_transcribe_audio(self, transcribe_action, reader, buff_size, callback):
        """Convenience function to create a transcribe stream that reads
        audio from the given reader in buff_size chunks. The provided
        callback is called with transcribe results as they become
        available. This function blocks until the streaming is complete."""
        # Check if we have a text or byte reader
        is_text = isinstance(reader, io.TextIOBase)

        # Set up a generator function to send the transcribe action
        # and audio data to Diatheke.
        def send_data():
            yield TranscribeInput(action=transcribe_action)
            while True:
                data = reader.read(buff_size)
                if (is_text and data == '') or (not is_text and data == b''):
                    # Reached the EOF
                    return

                # Send the audio
                yield TranscribeInput(audio=data)

        # Call the Transcribe method and send results to the callback
        for result in self._client.Transcribe(send_data()):
            callback(result)
