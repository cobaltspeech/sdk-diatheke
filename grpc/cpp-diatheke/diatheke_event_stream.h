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

#ifndef DIATHEKE_EVENT_STREAM_H
#define DIATHEKE_EVENT_STREAM_H

#include "diatheke.grpc.pb.h"
#include "diatheke_command_status.h"

#include <string>

namespace Diatheke
{

class Client;

class EventStream
{
public:
    using EventReader =
        grpc::ClientReader<cobaltspeech::diatheke::DiathekeEvent>;

    /*
     * Wrap the given gRPC reader in a new EventStream object. Most users
     * will not need to call this constructor directly, and should use the
     * Diatheke::Client::sessionEventStream() or
     * Diatheke::Session::eventStream() methods instead.
     */
    EventStream(const std::shared_ptr<EventReader> &reader,
                const std::shared_ptr<grpc::ClientContext> &ctx,
                const std::string &sessionID, Client *parent);

    ~EventStream();

    // Returns the ID of the parent session that started this stream.
    std::string sessionID();

    // Returns the parent client of this event stream
    Client *client();

    /*
     * Wait for the next event from Diatheke. Returns falses when there
     * are no more events to be sent. Blocks until either an event is
     * received or the stream is finished.
     */
    bool waitForEvent(cobaltspeech::diatheke::DiathekeEvent *event);

    /*
     * Close the event stream. This should be done after all events have
     * been received (i.e., waitForEvent() returned false).
     */
    void close();

    /*
     * Notify Diatheke when a command (requested by a CommandEvent in the
     * event stream) is completed. This method should ALWAYS be called
     * after receiving a CommandEvent, even if it is not called immediately
     * (e.g., a long running task will could potentially call it minutes after
     * receiving the initial CommandEvent). This allows Diatheke to update
     * the conversation state.
     */
    void commandFinished(const CommandStatus &status);

private:
    std::shared_ptr<EventReader> mStream;
    std::shared_ptr<grpc::ClientContext> mContext;
    std::string mSessionID;
    Client *mClient;

    // Disable copy construction and assignments
    EventStream(const EventStream &other);
    EventStream &operator=(const EventStream &other);
};

} // namespace Diatheke

#endif // DIATHEKE_EVENT_STREAM_H
