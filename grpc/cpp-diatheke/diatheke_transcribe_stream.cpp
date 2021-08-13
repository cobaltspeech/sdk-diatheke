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

#include "diatheke_transcribe_stream.h"

#include "diatheke_client_error.h"

namespace Diatheke
{
TranscribeStream::TranscribeStream(
    const std::shared_ptr<grpc::ClientContext> &ctx,
    const std::shared_ptr<GRPCReaderWriter> &stream)
    : mContext(ctx), mStream(stream)
{
}

TranscribeStream::~TranscribeStream() {}

bool TranscribeStream::sendAudio(const std::string &data)
{
    // Set up the request and write to the input stream
    cobaltspeech::diatheke::TranscribeInput request;
    request.set_audio(data);
    return mStream->Write(request);
}

bool TranscribeStream::sendAction(const cobaltspeech::diatheke::TranscribeAction &action)
{
    // Set up the request and write to the input stream
    cobaltspeech::diatheke::TranscribeInput request;
    *(request.mutable_action()) = action;
    return mStream->Write(request);
}

void TranscribeStream::sendFinished()
{
    if (!mStream->WritesDone())
    {
        throw ClientError("failed to finish sending");
    }
}

bool TranscribeStream::receiveResult(cobaltspeech::diatheke::TranscribeResult *result)
{
    return mStream->Read(result);
}

void TranscribeStream::close()
{
    grpc::Status status = mStream->Finish();
    if(!status.ok())
    {
        throw ClientError(status);
    }
}

TranscribeStream::GRPCReaderWriter *TranscribeStream::getStream() { return mStream.get(); }

} // namespace Diatheke
