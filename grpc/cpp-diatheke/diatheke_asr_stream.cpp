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

#include "diatheke_asr_stream.h"

#include "diatheke_client_error.h"

namespace Diatheke
{
ASRStream::ASRStream(const std::shared_ptr<ReaderWriter> &stream,
                     const std::shared_ptr<grpc::ClientContext> &ctx)
    : mStream(stream), mContext(ctx)
{
}

ASRStream::~ASRStream() {}

void ASRStream::pushAudio(const char *audio, size_t sizeInBytes)
{
    // Setup the request and write to the input stream
    cobaltspeech::diatheke::ASRRequest request;
    request.set_audio(audio, sizeInBytes);
    if (!mStream->Write(request))
    {
        throw ClientError(
            "could not push audio in ASRStream - input stream closed");
    }
}

void ASRStream::finishAudio()
{
    if (!mStream->WritesDone())
    {
        throw ClientError("unsuccessful writing audio data");
    }
}

bool ASRStream::waitForResult(cobaltspeech::diatheke::ASRResponse *result)
{
    return mStream->Read(result);
}

void ASRStream::close()
{
    grpc::Status status = mStream->Finish();
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

} // namespace Diatheke
