# -*- coding: utf-8 -*-
#
# Copyright(2020) Cobalt Speech and Language Inc.
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

from diatheke_pb2 import ASRInput
from queue import Queue
import threading
import io


class ASRStream(object):
    """ASRStream represents a stream of audio data sent from the client
    to Diatheke for the purpose of speech recognition."""

    def __init__(self, client_stub):
        # The Python implementation of gRPC is a little different from other
        # languages. Rather than having send and receive methods for the stream,
        # request streams in Python take an iterator or generator function. This
        # is a little inconvenient for how we want our SDK to be structured, so
        # we use our own stream class (which is an iterator) in conjunction with
        # some threading to make this work.

        # Set up a queue to transfer data from the write function to the
        # __next__ function.
        self._data_queue = Queue(maxsize=1)

        # Set up a lock and variable to handle checking whether a result is
        # available or not.
        self._lock = threading.Lock()
        self._result = None

        # They way client gRPC streaming works in Python is that the streaming
        # method accepts an iterator to send many messages to the server until
        # it is done, at which point the server returns a single response. The
        # function call blocks until this happens. To avoid blocking, we set
        # up a thread to run the StreamASR function with this class
        # as the iterator argument to that function.
        self._stream_thread = threading.Thread(target=self._run_stream,
                                               args=(client_stub,),
                                               daemon=True)
        self._stream_thread.start()

    def __iter__(self):
        return self

    def __next__(self):
        # Note this function is being called from the thread that the actual
        # stream is running on (i.e., from self._stream_thread).

        # Wait for data from the queue. The queue will block until
        # data is available.
        request = self._data_queue.get()
        if request is None:
            # This is our sentinel to indicate the iterator should finish,
            # which is done by raising the StopIteration exception. A side
            # effect of this is that if a user sends 'None' to one of the
            # send functions, it will also end the stream.
            raise StopIteration

        return request

    def _run_stream(self, client_stub):
        # Give the stream as the iterable the gRPC function requires.
        result = client_stub.StreamASR(self)
        self._set_result(result)

    def _has_result(self):
        with self._lock:
            return self._result != None

    def _set_result(self, result):
        with self._lock:
            self._result = result

    def send_audio(self, audio_bytes):
        """Send the given audio bytes to Diatheke for transcription.

        If this function returns False, the server has closed the stream
        and result() should be called to get the final ASR result."""
        # Check if there is a result yet
        if self._has_result():
            return False

        # Send the request
        req = ASRInput(audio=audio_bytes)
        self._data_queue.put(req)
        return True

    def send_token(self, token):
        """Send the given session token to Diatheke to update the
        speech recognition context. The session token must first be
        sent on the ASR stream before any audio will be recognized.
        If the stream was created using client.new_session_asr_stream(),
        the first token was already sent.

        If this function returns False, the server has closed the stream
        and result() should be called to get the final ASR result."""
        # Check if there is a result yet
        if self._has_result():
            return False

        # Send the request
        req = ASRInput(token=token)
        self._data_queue.put(req)
        return True

    def result(self):
        """Returns the result of speech recognition. This function may
        be called to end the audio stream early, which will force a
        transcription based on the audio received until this point, or
        in response to receiving False from send_audio() or send_token()."""
        # Check if the stream has already returned a result. If not,
        # force it to stop by sending our sentinel iterator value.
        if not self._has_result():
            self._data_queue.put(None)

        # Wait for the streaming thread to finish and return the result.
        self._stream_thread.join()
        return self._result


def read_ASR_audio(stream, reader, buffSize):
    """Convenience function to send audio from the given reader
    to the stream until a result is returned. Data is sent in
    chunks defined by buffSize."""
    # Check if we have a text reader
    if isinstance(reader, io.TextIOBase):
        is_text = True
    else:
        # Otherwise we assume we have a byte reader
        is_text = False

    while True:
        # Read the next chunk of data
        data = reader.read(buffSize)
        if (is_text and data == '') or (not is_text and data == b''):
            # Reached the EOF
            break

        # Send the audio
        if not stream.send_audio(data):
            break

    return stream.result()


def write_TTS_audio(stream, writer):
    """Convenience function to receive audio from the given
    TTS stream and send it to the writer until there is no
    more audio to receive."""
    # Check if we have a text writer
    if isinstance(writer, io.TextIOBase):
        is_text = True
    else:
        # Otherwise we assume we have a byte writer
        is_text = False

    for data in stream:
        if is_text:
            # Convert the text to a string before writing.
            writer.write(str(data.audio))
        else:
            writer.write(data.audio)
