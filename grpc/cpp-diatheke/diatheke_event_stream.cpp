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

#include "diatheke_event_stream.h"

#include "diatheke_client.h"
#include "diatheke_client_error.h"

namespace Diatheke
{
EventStream::EventStream(const std::shared_ptr<EventReader> &reader,
                         const std::shared_ptr<grpc::ClientContext> &ctx,
                         const std::string &sessionID, Client *parent)
    : mStream(reader), mContext(ctx), mSessionID(sessionID), mClient(parent)
{
}

EventStream::~EventStream() {}

std::string EventStream::sessionID() { return mSessionID; }

Client *EventStream::client() { return mClient; }

bool EventStream::waitForEvent(cobaltspeech::diatheke::DiathekeEvent *event)
{
    return mStream->Read(event);
}

void EventStream::close()
{
    grpc::Status status = mStream->Finish();
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

void EventStream::commandFinished(const CommandStatus &status)
{
    if (!mClient)
    {
        return;
    }

    mClient->commandFinished(mSessionID, status);
}

} // namespace Diatheke
