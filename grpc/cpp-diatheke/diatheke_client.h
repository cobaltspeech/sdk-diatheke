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

#ifndef DIATHEKE_CLIENT_H
#define DIATHEKE_CLIENT_H

#include "diatheke_asr_stream.h"
#include "diatheke_audio_input_stream.h"
#include "diatheke_audio_reply_stream.h"
#include "diatheke_command_status.h"
#include "diatheke_event_stream.h"
#include "diatheke_tts_stream.h"

#include <memory>
#include <string>
#include <vector>

namespace Diatheke
{

class Client
{
public:
    /*
     * Create a new client that is connected to a Diatheke server instance
     * running at the given url. The url should include the port number.
     * If insecure is false (default), the client will use TLS/SSL to
     * communicate with the server. Otherwise the connection is insecure.
     * Note that the server must also be running with TLS/SSL for the
     * secure connection to succeed.
     */
    Client(const std::string &url, bool insecure = false);
    ~Client();

    // Return the version of the Diatheke server.
    std::string diathekeVersion();

    // List the models availabe on the Diatheke server.
    std::vector<std::string> listModels();

    // Requests a new session for the given model and returns the session ID.
    std::string newSession(const std::string &model);

    /*
     * End the given session on the server. This will free up the resources
     * associated with the session and end any associated event streams.
     */
    void endSession(const std::string &sessionID);

    // Create a new event stream for the given session.
    std::unique_ptr<EventStream>
    sessionEventStream(const std::string &sessionID);

    /*
     * Notify Diatheke when a command has completed. The initial command
     * request will come as part of a DiathekeEvent in a session's event stream.
     * This method should always be called after receiving a command event when
     * the command is completed (even if it is a long running command).
     */
    void commandFinished(const std::string &sessionID,
                         const CommandStatus &result);

    /*
     * Begin an audio input stream for the given session. As the audio is
     * recognized, Diatheke will respond with appropriate events on the
     * session's event stream. Only one stream per session should be created.
     */
    std::unique_ptr<AudioInputStream>
    streamAudioInput(const std::string &sessionID);

    /*
     * Create a stream to receive output audio from Diatheke specifically
     * for the given session. The stream will use include start and end
     * messages to indicate when a section of audio for a group of text
     * begins and ends. Only one stream per session should be created.
     */
    std::unique_ptr<AudioReplyStream>
    streamAudioReplies(const std::string &sessionID);

    // Push text to Diatheke as part of a conversation for the given session.
    void pushText(const std::string &sessionID, const std::string &text);

    /*
     * Run streaming speech recognition unrelated to a session using the
     * given model.
     */
    std::unique_ptr<ASRStream> streamASR(const std::string &model);

    /*
     * Run streaming text-to-speech unrelated to a session for the given
     * text, using the specified model.
     */
    std::unique_ptr<TTSStream> streamTTS(const std::string &model,
                                         const std::string &text);

    /*
     * Set a timeout for server requests in milliseconds. A timeout value of
     * zero indicates no timeout. The default is 30000 (i.e., 30 seconds).
     * NOTE: The request timeout does not apply to streams.
     */
    void setRequestTimeout(unsigned int milliseconds);

private:
    using DiathekeGRPC = cobaltspeech::diatheke::Diatheke;
    std::unique_ptr<DiathekeGRPC::Stub> mStub;
    unsigned int mTimeout;

    // Disable copy construction and assignments.
    Client(const Client &other);
    Client &operator=(const Client &other);

    // Convenience functions
    void setContextDeadline(grpc::ClientContext &ctx);
};

} // namespace Diatheke

#endif
