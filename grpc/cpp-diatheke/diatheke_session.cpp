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

#include "diatheke_session.h"

// Alias to make this more manageable
using EventReader = grpc::ClientReader<cobaltspeech::diatheke::DiathekeEvent>;

namespace Diatheke
{

Session::Session(const std::string &id, Client *parent)
    : mClient(parent), mSessionID(id)
{
}

Session::~Session() {}

std::string Session::id() { return mSessionID; }

Client *Session::client() { return mClient; }

void Session::endSession()
{
    if (!mClient)
    {
        return;
    }

    mClient->endSession(mSessionID);
}

std::unique_ptr<EventStream> Session::eventStream()
{
    if (!mClient)
    {
        return nullptr;
    }

    return mClient->sessionEventStream(mSessionID);
}

void Session::commandFinished(const CommandStatus &result)
{
    if (!mClient)
    {
        return;
    }

    mClient->commandFinished(mSessionID, result);
}

std::unique_ptr<AudioInputStream> Session::streamAudioInput()
{
    if (!mClient)
    {
        return nullptr;
    }

    return mClient->streamAudioInput(mSessionID);
}

std::unique_ptr<AudioReplyStream> Session::streamAudioReplies()
{
    if (!mClient)
    {
        return nullptr;
    }

    return mClient->streamAudioReplies(mSessionID);
}

void Session::pushText(const std::string &text)
{
    if (!mClient)
    {
        return;
    }

    mClient->pushText(mSessionID, text);
}

} // namespace Diatheke
