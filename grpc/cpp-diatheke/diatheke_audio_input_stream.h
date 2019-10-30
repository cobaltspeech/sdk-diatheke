/*
 * Copyright (2019) Cobalt Speech and Language, Inc.
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

#ifndef DIATHEKE_AUDIO_INPUT_STREAM_H
#define DIATHEKE_AUDIO_INPUT_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>
#include <string>

namespace Diatheke
{

class AudioInputStream
{
public:
    using AudioWriter = grpc::ClientWriter<cobaltspeech::diatheke::AudioInput>;

    /*
     * Wrap the given gRPC reader in a new AudioInputStream object. Most users
     * will not need to call this constructor directly, and should use the
     * Diatheke::Client::streamAudioInput() or
     * Diatheke::Session::streamAudioInput() methods instead.
     */
    AudioInputStream(const std::shared_ptr<AudioWriter> &writer,
                     const std::shared_ptr<grpc::ClientContext> &ctx,
                     const std::string &parentSessionID);

    ~AudioInputStream();

    // Returns the ID of the session this stream belongs to.
    std::string sessionID();

    /*
     * Push the given audio data to the Diatheke server. The audio format
     * should match what is specified in the Diatheke server configuration.
     * If there is no audio data (i.e., sizeInBytes equals zero), the server
     * will treat this as EOF and end the stream. Subsequent calls to
     * pushAudio will fail.
     */
    void pushAudio(const char *audio, size_t sizeInBytes);

    /*
     * Notify Diathake that no more audio is coming from the client. It is
     * an error to call pushAudio() after calling this method.
     */
    void finished();

private:
    std::shared_ptr<AudioWriter> mStream;
    std::shared_ptr<grpc::ClientContext> mContext;
    std::string mSessionID;
};

} // namespace Diatheke

#endif // DIATHEKE_AUDIO_INPUT_STREAM_H
