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

#include "diatheke_audio_reply_stream.h"

#include "diatheke_client_error.h"

namespace Diatheke
{
AudioReplyStream::AudioReplyStream(
    const std::shared_ptr<AudioReplyReader> &reader,
    const std::shared_ptr<grpc::ClientContext> &ctx,
    const std::string &parentSessionID)
    : mStream(reader), mContext(ctx), mSessionID(parentSessionID)
{
}

AudioReplyStream::~AudioReplyStream() {}

std::string AudioReplyStream::sessionID() { return mSessionID; }

bool AudioReplyStream::waitForReply(cobaltspeech::diatheke::AudioReply *data)
{
    return mStream->Read(data);
}

void AudioReplyStream::close()
{
    grpc::Status status = mStream->Finish();
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

AudioReplyStream::AudioReplyStream(const AudioReplyStream &other)
{
    // Do nothing. Copy construction is disabled.
}

AudioReplyStream &AudioReplyStream::operator=(const AudioReplyStream &other)
{
    // Do nothing. Assignments are disabled.
    return *this;
}

} // namespace Diatheke
