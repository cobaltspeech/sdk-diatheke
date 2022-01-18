/*
 * Copyright (2021-present) Cobalt Speech and Language, Inc. All rights reserved.
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

#include <atomic>
#include <thread>

namespace Diatheke
{

/* Private data struct */
struct ASRStreamPrivate
{
    grpc::ClientContext context;
    cobaltspeech::diatheke::ASRResult result;
    std::shared_ptr<ASRStream::GRPCWriter> stream;
    std::atomic_bool hasResult;
    std::thread resultThread;
    grpc::Status status;

    ~ASRStreamPrivate()
    {
        if (!hasResult.load()) {
            // This indicates that the stream hasn't returned
            // yet, so we need to force it to close by
            // cancelling the stream's context.
            context.TryCancel();
        }
        
        // Make sure the thread has joined
        join();
    }

    void startResultThread()
    {
        // Wait for results on a separate thread.
        mJoined = false;
        resultThread = std::thread([](ASRStreamPrivate *data){
            data->status = data->stream->Finish();
            data->hasResult = true;
        }, this);
    }

    void join()
    {
        if (mJoined) {
            return;
        }

        mJoined = true;
        resultThread.join();
    }

private:
    bool mJoined;
};

ASRStream::ASRStream(cobaltspeech::diatheke::Diatheke::Stub *stub)
    : dPtr(std::make_shared<ASRStreamPrivate>())
{
    dPtr->stream = stub->StreamASR(&dPtr->context, &dPtr->result);
    dPtr->hasResult = false;
    dPtr->startResultThread();
}

ASRStream::~ASRStream() {}

bool ASRStream::sendAudio(const std::string &data)
{
    // Set up the request and write to the input stream
    cobaltspeech::diatheke::ASRInput request;
    request.set_audio(data);
    if (!dPtr->stream->Write(request)) {
        return false;
    }

    return !dPtr->hasResult.load();
}

bool ASRStream::sendToken(const cobaltspeech::diatheke::TokenData &token)
{
    // Set up the request and write to the input stream
    cobaltspeech::diatheke::ASRInput request;
    *(request.mutable_token()) = token;
    if (!dPtr->stream->Write(request)) {
        return false;
    }

    return !dPtr->hasResult.load();
}

cobaltspeech::diatheke::ASRResult ASRStream::result()
{
    // If Diatheke hasn't already sent the result,
    // notify it that no more writes are coming, which
    // should force a result.
    if (!dPtr->hasResult.load()) {
        dPtr->stream->WritesDone();
    }

    // Wait for the result to come back.
    dPtr->join();

    // Check the status and return the result.
    if (!dPtr->status.ok()) {
        throw Diatheke::ClientError(dPtr->status);
    }

    return dPtr->result;
}

ASRStream::GRPCWriter *ASRStream::getStream() { return dPtr->stream.get(); }

} // namespace Diatheke
