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

from diatheke_pb2 import AudioInput, ASRRequest
from queue import Queue
import threading


class AudioInputStream(object):
    """AudioInputStream represents an ASR stream for a session. Clients use
    this stream to push audio to the server for recognition.
    """

    def __init__(self, client_stub, session_id):
        """Creates a new stream using the given gRPC client stub and session ID.
        Most users do not need to call this constructor directly, and should
        instead use the client's stream_audio_input method to create this stream.
        """

        # The Python implementation of gRPC is a little different from other
        # languages. Rather than having send and receive methods for the stream,
        # request streams in Python take an iterator or generator function. This
        # is a little inconvenient for how we want our SDK to be structured, so
        # we use our own stream class (which is an iterator) in conjunction with
        # some threading to make this work.

        # Set up a queue to transfer data from the write function to the
        # __next__ function.
        self.data_queue = Queue(maxsize=1)
        self.session_id = session_id
        self._id_sent = False
        self._is_finished = False

        # Set up a thread to run the StreamAudioInput function with this class
        # as the iterator argument to that function.
        self._stream_thread = threading.Thread(target=client_stub.StreamAudioInput,
                                               args=(self,))
        self._stream_thread.start()

    def __iter__(self):
        return self

    def __next__(self):
        # Note this function is being called from the thread that the actual
        # stream is running on (i.e., from self._stream_thread).

        # Create a new request object to be sent
        request = AudioInput()

        if self._id_sent:
            # Wait for audio data from the queue. The queue will block until
            # data is available.
            audio_data = self.data_queue.get()
            if audio_data is None:
                # This is our sentinel to indicate the iterator should finish,
                # which is done by raising the StopIteration exception. A side
                # effect of this is that if a user sends 'None' to the write
                # function, it will also end the stream.
                raise StopIteration

            # Add the data to the request.
            request.data = audio_data

        else:
            # Send the session id. The API requires the session id be sent
            # on the stream first, which is why _id_sent is set to false in
            # the constructor.
            request.session_id = self.session_id
            self._id_sent = True

        return request

    def write(self, audio_data):
        """Sends the given audio data to the Diatheke server over the gRPC stream."""

        # Check if the stream is already done
        if self._is_finished:
            raise RuntimeError("Attempted write on closed AudioInputStream.")

        # Write the data to the queue to be consumed by the gRPC stream as it is
        # ready (happens in the __next__ function). This will block if the queue
        # is already full.
        self.data_queue.put(audio_data)

    def finish(self):
        """Notify the server that no more data will be sent, which effectively
        ends the stream. Calls to write() will fail after calling this method.
        """
        if self._is_finished:
            # Ignore if we already called this function.
            return

        self._is_finished = True

        # Push a None value so the __next__ function will know we are done.
        self.data_queue.put(None)


class ASRStream(object):
    """ASRStream represents a speech recognition stream unrelated to a session."""

    def __init__(self, client_stub, cubic_model_id):
        """Creates a new stream using the given gRPC client stub and cubic model ID.
        Most users do not need to call this constructor directly, and should
        instead use the client's stream_asr method to create this stream.
        """

        # Set up a queue to transer data from the write function to the
        # __next__ function.
        self.data_queue = Queue(maxsize=1)
        self.model_id = cubic_model_id
        self._id_sent = False
        self._is_finished = False

        # Fortunately, unlike the AudioInputStream which has a client-stream,
        # creating a bidirectional gRPC stream in python does not block, so
        # we don't need to call it on a separate thread.
        self.result_stream = client_stub.StreamASR(self)

    def __iter__(self):
        return self

    def __next__(self):

        # Create a new request object to be sent
        request = ASRRequest()

        if self._id_sent:
            # Wait for audio data from the queue. The queue will block until
            # data is available
            audio_data = self.data_queue.get()
            if audio_data is None:
                # This is our sentinel to indicate the iterator should finish,
                # which is done by raising the StopIteration exception. A side
                # effect of this is that if a user sends 'None' to the write
                # function, it will also end the stream.
                raise StopIteration

            # Add the data to the request
            request.audio = audio_data

        else:
            # Send the cubic model id. The API requires the model id to be
            # sent on the stream first, which is why _id_sent is set to false
            # in the constructor.
            request.model = self.model_id
            self._id_sent = True

        return request

    def write(self, audio_data):
        """Sends the given audio data to the Diatheke server over the gRPC stream."""

        # Check if the stream is already done
        if self._is_finished:
            raise RuntimeError("Attempted write on closed AudioInputStream.")

        # Write the data to the queue to be consumed by the gRPC stream as it is
        # ready (happens in the __next__ function). This will block if the queue
        # is already full.
        self.data_queue.put(audio_data)

    def audio_finished(self):
        """Notify the server that no more data will be sent, which effectively
        ends the stream. Calls to write() will fail after calling this method.
        """
        if self._is_finished:
            # Ignore if we already called this function.
            return

        self._is_finished = True

        # Push a None value so the __next__ function will know we are done.
        self.data_queue.put(None)
