/*
 * Copyright (2020) Cobalt Speech and Language, Inc.
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

#include "diatheke_asr_stream.h"

#include "diatheke_client_error.h"

namespace Diatheke
{
ASRStream::ASRStream(
    const std::shared_ptr<grpc::ClientContext> &ctx,
    const std::shared_ptr<cobaltspeech::diatheke::ASRResult> &result,
    const std::shared_ptr<GRPCWriter> &stream)
    : mContext(ctx), mResult(result), mStream(stream)
{
}

ASRStream::~ASRStream() {}

bool ASRStream::sendAudio(const std::string &data)
{
    // Set up the request and write to the input stream
    cobaltspeech::diatheke::ASRInput request;
    request.set_audio(data);
    if (!mStream->Write(request))
    {
        return false;
    }

    return true;
}

bool ASRStream::sendToken(const cobaltspeech::diatheke::TokenData &token)
{
    // Set up the request and write to the input stream
    cobaltspeech::diatheke::ASRInput request;
    *(request.mutable_token()) = token;
    if (!mStream->Write(request))
    {
        return false;
    }

    return true;
}

cobaltspeech::diatheke::ASRResult ASRStream::result()
{
    mStream->WritesDone();
    grpc::Status status = mStream->Finish();
    if (!status.ok())
    {
        throw Diatheke::ClientError(status);
    }

    return *mResult;
}

ASRStream::GRPCWriter *ASRStream::getStream() { return mStream.get(); }

AudioReader::AudioReader() {}
AudioReader::~AudioReader() {}

cobaltspeech::diatheke::ASRResult
ReadASRAudio(ASRStream &stream, AudioReader *reader, size_t buffSize)
{
    std::string buffer;
    buffer.resize(buffSize);
    while (true)
    {
        // Pull audio data from the reader
        size_t bytesRead = reader->readAudio(&buffer[0], buffer.size());
        if (bytesRead == 0)
        {
            break;
        }

        // Send the audio to the stream
        if (!stream.sendAudio(buffer.substr(0, bytesRead)))
        {
            break;
        }
    }

    // Return the result
    return stream.result();
}

} // namespace Diatheke
