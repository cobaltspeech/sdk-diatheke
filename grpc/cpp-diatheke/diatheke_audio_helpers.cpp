/*
 * Copyright (2021) Cobalt Speech and Language, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diatheke_audio_helpers.h"

#include "diatheke_client_error.h"

#include <exception>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace Diatheke
{

AudioReader::AudioReader() {}
AudioReader::~AudioReader() {}

AudioWriter::AudioWriter() {}
AudioWriter::~AudioWriter() {}

cobaltspeech::diatheke::ASRResult
ReadASRAudio(ASRStream &stream, AudioReader *reader, size_t buffSize)
{
    std::vector<char> buffer(buffSize);
    while (true)
    {
        // Pull audio data from the reader
        size_t bytesRead = reader->readAudio(buffer.data(), buffSize);
        if (bytesRead == 0)
        {
            break;
        }

        // Send the audio to the stream
        if (!stream.sendAudio(std::string(buffer.data(), bytesRead)))
        {
            break;
        }
    }

    // Return the result
    return stream.result();
}

/*
 * Helper class to capture the first exception that occurs on multiple
 * threads. Also allows each thread to check if there was an error on
 * any other thread, and rethrows the first exception encountered from
 * the thread where all the other threads were spawned.
 */
class MultiThreadExceptionHelper {
private:
    std::mutex mLock;
    bool mSet;
    std::exception_ptr mError;

public:
    MultiThreadExceptionHelper() :
        mSet(false) {}
    
    ~MultiThreadExceptionHelper() {}

    /*
     * Call from the thread where the exception occurred.
     * This function is thread-safe.
     */
    void captureException()
    {
        mLock.lock();

        if (!mSet) {
            // Capture the exception on the calling thread.
            mSet = true;
            mError = std::current_exception();
        }

        mLock.unlock();
    }

    /*
     * Returns true if an error has been encountered on one
     * of the threads. This function is thread-safe.
     */
    bool isSet()
    {
        mLock.lock();
        bool tmp = mSet;
        mLock.unlock();
        return tmp;
    }

    /*
     * Call from the thread where this class was instantiated,
     * after all threads using it have been join()-ed. Will
     * rethrow the first exception that was encountered.
     */
    void rethrow()
    {
        std::rethrow_exception(mError);
    }
};

void ReadTranscribeAudio(TranscribeStream &stream, AudioReader *reader, size_t buffSize,
                         std::function<void(const cobaltspeech::diatheke::TranscribeResult&)> callback)
{
    // Used to capture exceptions on the threads.
    MultiThreadExceptionHelper err;

    // Handle reading audio on a separate thread.
    std::thread audioThread([&](){
        std::vector<char> buffer(buffSize);
        try
        {
            while(true)
            {
                // Pull audio from the reader
                size_t bytesRead = reader->readAudio(buffer.data(), buffSize);
                if (bytesRead == 0 || err.isSet())
                {
                    stream.sendFinished();
                    return;
                }

                // Send audio to the stream
                if (!stream.sendAudio(std::string(buffer.data(), bytesRead)) || err.isSet())
                {
                    return;
                }
            }
        }
        catch(...)
        {
            err.captureException();
        }
    });

    // Handle forwarding results on a separate thread.
    std::thread resultsThread([&](){
        try
        {
            while(true)
            {
                // Wait for the next result
                cobaltspeech::diatheke::TranscribeResult result;
                if(!stream.receiveResult(&result) || err.isSet())
                {
                    return;
                }

                // Pass the result to the callback
                callback(result);
            }
        }
        catch(...)
        {
            err.captureException();
        }
    });

    // Wait for the threads to finish, then close the stream.
    audioThread.join();
    resultsThread.join();
    stream.close();

    // If there was an error, rethrow it on the calling thread.
    if (err.isSet())
    {
        err.rethrow();
    }
}

void WriteTTSAudio(TTSStream &stream, AudioWriter *writer)
{
    // Wait for the next audio chunk
    std::string buffer;
    while (stream.receiveAudio(buffer))
    {
        // Write the audio chunk
        size_t bytesWritten = writer->writeAudio(buffer.data(), buffer.size());
        if (bytesWritten != buffer.size())
        {
            throw ClientError("AudioWriter did not write all data");
        }
    }
}

} // namespace Diatheke
