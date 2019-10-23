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

#include "diatheke_tts_stream.h"

#include "diatheke_client_error.h"

namespace Diatheke
{
TTSStream::TTSStream(const std::shared_ptr<Reader> &stream,
                     const std::shared_ptr<grpc::ClientContext> &ctx)
    : mStream(stream), mContext(ctx)
{
}

TTSStream::~TTSStream() {}

bool TTSStream::waitForAudio(cobaltspeech::diatheke::TTSResponse *response)
{
    return mStream->Read(response);
}

void TTSStream::close()
{
    grpc::Status status = mStream->Finish();
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

} // namespace Diatheke
