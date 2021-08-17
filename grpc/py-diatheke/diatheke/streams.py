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

from diatheke_pb2 import ASRInput, TranscribeInput
from queue import Queue
import threading
import io


class ASRStream(object):
    """ASRStream represents a stream of audio data sent from the client
    to Diatheke for speech recognition in the context of a session."""

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
    """Deprecated. Use client.read_asr_audio() instead."""
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
    """Deprecated. Use client.write_tts_audio() instead."""
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

class TranscribeStream(object):
    """TranscribeStream represents a bidirectional stream where audio is
    sent to Diatheke and transcriptions are returned to the client. This
    differs from an ASRStream in purpose - where ASRStream accepts audio
    to continue a conversation with the system, TranscribeStream accepts
    audio solely for the purpose of getting a transcription that the client
    can use for any purpose (e.g., note taking, send a message, etc.)."""

    def __init__(self, client_stub):
        # The Python implementation of gRPC is a little different from
        # other languages. Rather than having send and receive methods
        # for the stream, the bidirectional stream takes an iterator or
        # generator function, and returns an iterable. This is a little
        # inconvenient for how we want our SDK to be structured, so we
        # use our own stream class with some threading to make this work.

        # Set up queues to transfer data
        input = Queue(maxsize=1)
        output = Queue(maxsize=1)
        self._input_queue = input
        self._output_queue = output

        # Use a flag (and lock) to indicate when the stream is closed.
        lock = threading.Lock()
        doneFlag = False
        def is_done():
            nonlocal doneFlag
            with lock:
                if doneFlag:
                    return True
                return False
        self._is_done = is_done

        # Generator function to pass data from the input queue to the
        # streaming method.
        def wait_for_audio():
            while not is_done():
                # Wait for data from the queue, which will block until data
                # is available.
                request = input.get()
                if request is None:
                    # This is our sentinel to indicate that the generator
                    # should finish, which will end the stream.
                    return
                yield request

        # Function to run the gRPC method.
        def run_stream():
            nonlocal doneFlag
            for result in client_stub.Transcribe(wait_for_audio()):
                output.put(result)

            # Set the done flag
            with lock:
                doneFlag = True

            # Send None on the output to indicate the stream has finished.
            output.put(None)

        # Start the stream on its own thread.
        self._stream_thread = threading.Thread(target=run_stream)
        self._stream_thread.start()

    def _send(self, request):
        # Check if the stream has closed.
        if self._is_done():
            return False

        # Send the request to the streaming thread.
        self._input_queue.put(request)
        return True

    def send_action(self, action):
        """Send the given TranscribeAction to Diatheke to update the speech
        recognition context. The action must first be sent on the stream
        before any audio will be recognized. If the stream was created using
        client.new_transcribe_stream(), this action was already sent.
        
        If this function returns False, the server has closed the stream, and
        no further attempts should be made to send an action or audio data to
        the server."""
        return self._send(TranscribeInput(action=action))

    def send_audio(self, audio_bytes):
        """Send the given audio data to Diatheke for transcription.
        
        If this function returns False, the server has closed the stream,
        and no further attempts should be made to send an action or audio
        data to the server.
        
        It is thread-safe to call this method while also calling
        receive_result()."""
        return self._send(TranscribeInput(audio=audio_bytes))
    
    def send_finished(self):
        """Tell the server that no more data will be sent over this stream.
        It is an error to call send_audio() or send_action() after calling
        this."""
        self._send(None)

    def receive_result(self):
        """Wait for the next available TranscribeResult from the server.
        Returns None when there are no more results to receive.
        
        It is thread-safe to call this method while also calling
        send_audio()."""
        # Pull the next result from from the streaming thread.
        return self._output_queue.get()
