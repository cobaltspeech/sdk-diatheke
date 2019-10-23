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

#include "diatheke_audio_input_stream.h"

#include "diatheke_client_error.h"

namespace Diatheke
{

AudioInputStream::AudioInputStream(
    const std::shared_ptr<AudioWriter> &writer,
    const std::shared_ptr<grpc::ClientContext> &ctx,
    const std::string &parentSessionID)
    : mStream(writer), mContext(ctx), mSessionID(parentSessionID)
{
}

AudioInputStream::~AudioInputStream() {}

std::string AudioInputStream::sessionID() { return mSessionID; }

void AudioInputStream::pushAudio(const char *audio, size_t sizeInBytes)
{
    if (sizeInBytes == 0)
    {
        return;
    }

    // Setup the request and write to the input stream.
    cobaltspeech::diatheke::AudioInput request;
    request.set_data(audio, sizeInBytes);
    if (!mStream->Write(request))
    {
        throw ClientError("could not push audio - input stream is closed");
    }
}

void AudioInputStream::finished()
{
    // Notify Diatheke that no more data will be coming.
    mStream->WritesDone();

    // Get the final gRPC status.
    grpc::Status status = mStream->Finish();
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

} // namespace Diatheke
