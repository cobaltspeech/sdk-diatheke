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

#ifndef DIATHEKE_SESSION_H
#define DIATHEKE_SESSION_H

#include "diatheke_client.h"

#include <memory>
#include <string>

namespace Diatheke
{

/*
 * The session class is provided as a convenience for working with
 * methods that require a session ID.
 */
class Session
{
public:
    // Create a session instance using the given session id and parent client.
    Session(const std::string &id, Client *parent);

    ~Session();

    // Returns the id of this session.
    std::string id();

    // Returns the parent client of this session.
    Client *client();

    // End this session.
    void endSession();

    // Returns an event stream for this session.
    std::unique_ptr<EventStream> eventStream();

    /*
     * Notify Diatheke when a command has completed. The initial command
     * request will come as part of a DiathekeEvent in a session's event stream.
     * This method should always be called after receiving a command event when
     * the command is completed (even if it is a long running command).
     */
    void commandFinished(const CommandStatus &result);

    /*
     * Create a stream to use to push audio input to Diatheke specifically
     * for this session. Transcriptions for the audio are not returned on this
     * stream, but are sent as RecognizeEvents in the session's event stream.
     * Transcriptions are automatically passed to Diatheke for processing.
     *
     * Only one audio input stream per session should be running at a time to
     * avoid confusing the speech recognition with multiple audio sources.
     */
    std::unique_ptr<AudioInputStream> streamAudioInput();

    /*
     * Create a stream to receive TTS output audio from Diatheke specifically
     * for this session. The stream will use include start and end messages
     * to indicate when a section of audio for a group of text begins and ends.
     */
    std::unique_ptr<AudioReplyStream> streamAudioReplies();

    /*
     * Push text to Diatheke as part of a conversation for the given session.
     *
     * This function may be used alone or in addition to an audio input
     * stream to drive the conversation with Diatheke. On the server side,
     * the audio input stream essentially calls this function automaticaly as
     * transcriptions are available.
     */
    void pushText(const std::string &text);

private:
    Client *mClient;
    std::string mSessionID;
};

} // namespace Diatheke

#endif // DIATHEKE_SESSION_H
