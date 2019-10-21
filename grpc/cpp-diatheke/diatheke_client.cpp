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

#include "diatheke_client.h"

#include "diatheke.grpc.pb.h"
#include "diatheke_client_error.h"

#include <chrono>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

namespace Diatheke
{

Client::Client(const std::string &url, bool insecure) : mTimeout(30000)
{
    /*
     * Quick runtime check to verify that the user has linked against
     * a version of protobuf that is compatible with the version used
     * to generate the c++ files.
     */
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Setup credentials
    std::shared_ptr<grpc::ChannelCredentials> creds;
    if (insecure)
    {
        creds = grpc::InsecureChannelCredentials();
    }
    else
    {
        creds = grpc::SslCredentials(grpc::SslCredentialsOptions());
    }

    // Create the channel and stub
    std::unique_ptr<DiathekeGRPC::Stub> tmp =
        DiathekeGRPC::NewStub(grpc::CreateChannel(url, creds));
    mStub.swap(tmp);
}

Client::~Client() {}

std::string Client::diathekeVersion()
{
    // Setup the request
    cobaltspeech::diatheke::Empty request;
    cobaltspeech::diatheke::VersionResponse response;

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Get the version from the server
    grpc::Status status = mStub->Version(&ctx, request, &response);
    if (!status.ok())
    {
        throw Diatheke::ClientError(status);
    }

    return response.server();
}

std::vector<std::string> Client::listModels()
{
    // Setup the request
    cobaltspeech::diatheke::Empty request;
    cobaltspeech::diatheke::ModelsResponse response;

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Get the list of models from the server
    grpc::Status status = mStub->Models(&ctx, request, &response);
    if (!status.ok())
    {
        throw Diatheke::ClientError(status);
    }

    // Package the models nicely in a vector
    std::vector<std::string> models;
    for (int i = 0; i < response.models_size(); i++)
    {
        models.push_back(response.models(i));
    }

    return models;
}

std::string Client::newSession(const std::string &model)
{
    // Setup the server request
    cobaltspeech::diatheke::NewSessionRequest request;
    request.set_model(model);

    cobaltspeech::diatheke::SessionID response;

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Get the session ID from the server
    grpc::Status status = mStub->NewSession(&ctx, request, &response);
    if (!status.ok())
    {
        throw Diatheke::ClientError(status);
    }

    return response.session_id();
}

void Client::endSession(const std::string &sessionID)
{
    // Setup the server request
    cobaltspeech::diatheke::SessionID request;
    request.set_session_id(sessionID);

    cobaltspeech::diatheke::Empty response;

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Send the request
    grpc::Status status = mStub->EndSession(&ctx, request, &response);
    if (!status.ok())
    {
        throw Diatheke::ClientError(status);
    }
}

std::unique_ptr<EventStream>
Client::sessionEventStream(const std::string &sessionID)
{
    // Setup the server request
    cobaltspeech::diatheke::SessionID request;
    request.set_session_id(sessionID);

    // We don't set a deadline on this context because we expect
    // this to be a long-lived stream.
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Create the stream reader
    std::shared_ptr<grpc::ClientReader<cobaltspeech::diatheke::DiathekeEvent>>
        reader(mStub->SessionEventStream(ctx.get(), request));

    // Return the event stream object
    return std::unique_ptr<EventStream>(
        new EventStream(reader, ctx, sessionID, this));
}

void Client::commandFinished(const std::string &sessionID,
                             const CommandStatus &result)
{
    // Setup the server request
    cobaltspeech::diatheke::CommandStatus request;
    request.set_session_id(sessionID);
    request.set_command_id(result.commandID());

    // Convert the status code
    switch (result.statusCode())
    {
    case CommandStatus::SUCCESS:
        request.set_return_status(
            cobaltspeech::diatheke::CommandStatus::SUCCESS);
        break;

    default:
    case CommandStatus::FAILURE:
        request.set_return_status(
            cobaltspeech::diatheke::CommandStatus::FAILURE);
        break;
    }

    // Add the output parameters
    CommandStatus::ParamMap::const_iterator iter;
    for (iter = result.outputParams().begin();
         iter != result.outputParams().end(); iter++)
    {
        (*request.mutable_output_parameters())[iter->first] = iter->second;
    }

    // Set the error message
    request.set_error_message_text(result.errorMessage());

    // Don't forget the internal data
    request.set_command_state_id(result.stateID());

    // Now setup the response and context
    cobaltspeech::diatheke::Empty response;
    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Send the request
    grpc::Status status = mStub->CommandFinished(&ctx, request, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

std::unique_ptr<AudioInputStream>
Client::streamAudioInput(const std::string &sessionID)
{
    // Prepare the response and context objects. We expect this to be a
    // long lived stream, so we don't set a deadline on the context.
    cobaltspeech::diatheke::Empty response;
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Create the stream
    std::shared_ptr<grpc::ClientWriter<cobaltspeech::diatheke::AudioInput>>
        writer(mStub->StreamAudioInput(ctx.get(), &response));

    // Send the first message with the session ID
    cobaltspeech::diatheke::AudioInput idMessage;
    idMessage.set_session_id(sessionID);
    if (!writer->Write(idMessage))
    {
        throw ClientError("could not start audio input stream for session " +
                          sessionID);
    }

    // Return the audio input stream object
    return std::unique_ptr<AudioInputStream>(
        new AudioInputStream(writer, ctx, sessionID));
}

std::unique_ptr<AudioReplyStream>
Client::streamAudioReplies(const std::string &sessionID)
{
    // Setup the server request
    cobaltspeech::diatheke::SessionID request;
    request.set_session_id(sessionID);

    // We don't set a deadline on this context because we expect
    // this to be a long-lived stream.
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Create the stream reader
    std::shared_ptr<grpc::ClientReader<cobaltspeech::diatheke::AudioReply>>
        reader(mStub->StreamAudioReplies(ctx.get(), request));

    // Return the AudioReplyStream object
    return std::unique_ptr<AudioReplyStream>(
        new AudioReplyStream(reader, ctx, sessionID));
}

void Client::pushText(const std::string &sessionID, const std::string &text)
{
    // Setup the server request
    cobaltspeech::diatheke::PushTextRequest request;
    request.set_session_id(sessionID);
    request.set_text(text);

    cobaltspeech::diatheke::Empty response;
    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Send the request
    grpc::Status status = mStub->PushText(&ctx, request, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

std::unique_ptr<ASRStream> Client::streamASR(const std::string &model)
{
    /*
     * Create the context. We need it to exist for the lifetime of the
     * stream, so we create it as a managed pointer. We don't set a
     * deadline on the context because we expect the stream to be long-
     * lived.
     */
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Create the stream
    std::shared_ptr<
        grpc::ClientReaderWriter<cobaltspeech::diatheke::ASRRequest,
                                 cobaltspeech::diatheke::ASRResponse>>
        stream(mStub->StreamASR(ctx.get()));

    // Send the first message (the model) to the server
    cobaltspeech::diatheke::ASRRequest request;
    request.set_model(model);
    if (!stream->Write(request))
    {
        throw ClientError("could not start streaming ASR");
    }

    // Return the ASRStream object
    return std::unique_ptr<ASRStream>(new ASRStream(stream, ctx));
}

std::unique_ptr<TTSStream> Client::streamTTS(const std::string &model,
                                             const std::string &text)
{
    /*
     * Create the context. We need it to exist for the lifetime of the
     * stream, so we create it as a managed pointer. We don't set a
     * deadline on the context because we expect the stream to be long-
     * lived.
     */
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Setup the request
    cobaltspeech::diatheke::TTSRequest request;
    request.set_model(model);
    request.set_text(text);

    // Create the stream
    std::shared_ptr<grpc::ClientReader<cobaltspeech::diatheke::TTSResponse>>
        reader(mStub->StreamTTS(ctx.get(), request));

    // Return the TTSStream object
    return std::unique_ptr<TTSStream>(new TTSStream(reader, ctx));
}

void Client::setRequestTimeout(unsigned int milliseconds)
{
    mTimeout = milliseconds;
}

Client::Client(const Client &other)
{
    // Do nothing. Copy construction is disabled.
}

Client &Client::operator=(const Client &other)
{
    // Do nothing. Assignments are disabled.
    return *this;
}

void Client::setContextDeadline(grpc::ClientContext &ctx)
{
    if (mTimeout == 0)
    {
        return;
    }

    std::chrono::system_clock::time_point deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(mTimeout);
    ctx.set_deadline(deadline);
}

} // namespace Diatheke
