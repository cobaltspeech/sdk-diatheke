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

#ifndef DIATHEKE_CLIENT_H
#define DIATHEKE_CLIENT_H

#include <map>
#include <memory>
#include <string>

#include <grpcpp/security/credentials.h>

#include "diatheke.grpc.pb.h"
#include "diatheke_asr_stream.h"
#include "diatheke_transcribe_stream.h"
#include "diatheke_tts_stream.h"

namespace Diatheke
{

/*
 * Client is an object used to interact with the Diatheke
 * gRPC API.
 */
class Client
{
public:
    /*
     * Create a new insecure client that is connected to a Diatheke server
     * instance running at the given url. Note that for security reasons
     * it is not recommended to use an insecure connection in production.
     */
    Client(const std::string &url);

    /*
     * Create a new secure client connected to a Diatheke server instance
     * running at the given url. The connection will use TLS/SSL to
     * communicate with the server. For the connection to succeed, the server
     * must also be using a TLS/SSL connection.
     *
     * Note that on Windows and Mac, the gRPC library does not find the
     * root CA certificates automatically, in which case the `pem_root_certs`
     * field of the options should be set to contain the PEM encoded
     * contents of the root CA cert.
     * (see https://grpc.github.io/grpc/cpp/structgrpc_1_1_ssl_credentials_options.html)
     */
    Client(const std::string &url, const grpc::SslCredentialsOptions &opts);

    ~Client();

    // Returns version information from the server.
    cobaltspeech::diatheke::VersionResponse version();

    // List the models available to the Diatheke server.
    cobaltspeech::diatheke::ListModelsResponse listModels();

    /*
     * Create a new Diatheke session using the specified
     * model ID.
     */
    cobaltspeech::diatheke::SessionOutput
    createSession(const std::string &modelID);

    /*
     * Create a new Diatheke session using the specified
     * model ID and custom wakeword. The wakeword will only
     * have an effect if the model has wakeword detection
     * enabled.
     */
    cobaltspeech::diatheke::SessionOutput
    createSessionWithWakeWord(const std::string &modelID,
                              const std::string &wakeword);

    /*
     * Clean up the given session token. Behavior is undefined
     * if the given token is used again after calling this
     * function.
     */
    void deleteSession(const cobaltspeech::diatheke::TokenData &token);

    /*
     * Send the given text to Diatheke and return an updated
     * session token.
     */
    cobaltspeech::diatheke::SessionOutput
    processText(const cobaltspeech::diatheke::TokenData &token,
                const std::string &text);

    /*
     * Send the given ASR result to Diatheke and return an
     * updated session token.
     */
    cobaltspeech::diatheke::SessionOutput
    processASRResult(const cobaltspeech::diatheke::TokenData &token,
                     const cobaltspeech::diatheke::ASRResult &result);

    /*
     * Send the given command result to Diatheke and return
     * an updated session token. This function should be
     * called in response to a command action Diatheke sent
     * previously.
     */
    cobaltspeech::diatheke::SessionOutput
    processCommandResult(const cobaltspeech::diatheke::TokenData &token,
                         const cobaltspeech::diatheke::CommandResult &result);

    /*
     * Change the current story for a Diatheke session.
     * Returns an updated session token.
     */
    cobaltspeech::diatheke::SessionOutput
    setStory(const cobaltspeech::diatheke::TokenData &token,
             const std::string &storyID,
             const std::map<std::string, std::string> &params);

    /*
     * Create a new stream to transcribe audio for the given
     * session token.
     */
    ASRStream
    newSessionASRStream(const cobaltspeech::diatheke::TokenData &token);

    /*
     * Create a new stream to receive TTS audio from Diatheke
     * based on the given ReplyAction.
     */
    TTSStream newTTSStream(const cobaltspeech::diatheke::ReplyAction &reply);

    /*
     * Create a new stream for transcriptions, usually in response to a
     * TranscribeAction.
     */
    TranscribeStream
    newTranscribeStream(const cobaltspeech::diatheke::TranscribeAction &action);

    /*
     * Set a timeout for server requests in milliseconds. A timeout value of
     * zero indicates no timeout. The default is 30000 (i.e., 30 seconds).
     * NOTE: The request timeout does not apply to streams.
     */
    void setRequestTimeout(unsigned int milliseconds);

private:
    using DiathekeGRPC = cobaltspeech::diatheke::Diatheke;
    std::shared_ptr<DiathekeGRPC::Stub> mStub;
    unsigned int mTimeout;

    // Convenience functions
    void setContextDeadline(grpc::ClientContext &ctx);

    cobaltspeech::diatheke::SessionOutput
    updateSession(const cobaltspeech::diatheke::SessionInput &request);
};

} // namespace Diatheke

#endif
