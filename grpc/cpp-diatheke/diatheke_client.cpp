/*
 * Copyright (2020) Cobalt Speech and Language, Inc.
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

static unsigned int defaultTimeout = 30000;

Client::Client(const std::string &url) : mTimeout(defaultTimeout)
{
    /*
     * Quick runtime check to verify that the user has linked against
     * a version of protobuf that is compatible with the version used
     * to generate the c++ files.
     */
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Set up credentials
    auto creds = grpc::InsecureChannelCredentials();

    // Create the channel and stub
    mStub = DiathekeGRPC::NewStub(grpc::CreateChannel(url, creds));
}

Client::Client(const std::string &url, const grpc::SslCredentialsOptions &opts) :
    mTimeout(defaultTimeout)
{
    /*
     * Quick runtime check to verify that the user has linked against
     * a version of protobuf that is compatible with the version used
     * to generate the c++ files.
     */
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Set up secure credentials
    auto creds = grpc::SslCredentials(opts);

    // Create the channel and stub
    mStub = DiathekeGRPC::NewStub(grpc::CreateChannel(url, creds));
}

Client::~Client() {}

cobaltspeech::diatheke::VersionResponse Client::version()
{
    // Set up the request
    cobaltspeech::diatheke::Empty request;
    cobaltspeech::diatheke::VersionResponse response;

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Get the version from the server
    grpc::Status status = mStub->Version(&ctx, request, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }

    return response;
}

cobaltspeech::diatheke::ListModelsResponse Client::listModels()
{
    // Set up the request
    cobaltspeech::diatheke::Empty request;
    cobaltspeech::diatheke::ListModelsResponse response;

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Get the list of models from the server
    grpc::Status status = mStub->ListModels(&ctx, request, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }

    return response;
}

cobaltspeech::diatheke::SessionOutput
Client::createSession(const std::string &modelID)
{
    // Set up the server request
    cobaltspeech::diatheke::SessionStart request;
    request.set_model_id(modelID);

    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Send and get a response
    cobaltspeech::diatheke::SessionOutput response;
    grpc::Status status = mStub->CreateSession(&ctx, request, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }

    return response;
}

void Client::deleteSession(const cobaltspeech::diatheke::TokenData &token)
{
    // Set up the server request
    cobaltspeech::diatheke::Empty response;
    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    grpc::Status status = mStub->DeleteSession(&ctx, token, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }
}

cobaltspeech::diatheke::SessionOutput
Client::processText(const cobaltspeech::diatheke::TokenData &token,
                    const std::string &text)
{
    // Set up the server request
    cobaltspeech::diatheke::SessionInput request;
    *(request.mutable_token()) = token;
    request.mutable_text()->set_text(text);

    return this->updateSession(request);
}

cobaltspeech::diatheke::SessionOutput
Client::processASRResult(const cobaltspeech::diatheke::TokenData &token,
                         const cobaltspeech::diatheke::ASRResult &result)
{
    // Set up the server request
    cobaltspeech::diatheke::SessionInput request;
    *(request.mutable_token()) = token;
    *(request.mutable_asr()) = result;

    return this->updateSession(request);
}

cobaltspeech::diatheke::SessionOutput Client::processCommandResult(
    const cobaltspeech::diatheke::TokenData &token,
    const cobaltspeech::diatheke::CommandResult &result)
{
    // Set up the server request
    cobaltspeech::diatheke::SessionInput request;
    *(request.mutable_token()) = token;
    *(request.mutable_cmd()) = result;

    return this->updateSession(request);
}

cobaltspeech::diatheke::SessionOutput
Client::setStory(const cobaltspeech::diatheke::TokenData &token,
                 const std::string &storyID,
                 const std::map<std::string, std::string> &params)
{
    // Set up the server request
    cobaltspeech::diatheke::SessionInput request;
    *(request.mutable_token()) = token;
    auto story = request.mutable_story();
    story->set_story_id(storyID);

    // Set parameters
    auto outputParams = story->mutable_parameters();
    for (auto iter = params.begin(); iter != params.end(); iter++)
    {
        (*outputParams)[iter->first] = iter->second;
    }

    return this->updateSession(request);
}

ASRStream
Client::newSessionASRStream(const cobaltspeech::diatheke::TokenData &token)
{
    /*
     * Create the context. We need it to exist for the lifetime of the
     * stream, so we create it as a managed pointer. We don't set a
     * deadline on the context because we expect the stream to be long-
     * lived.
     */
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Create a managed pointer to store the final result.
    std::shared_ptr<cobaltspeech::diatheke::ASRResult> result(
        new cobaltspeech::diatheke::ASRResult);

    // Create the gRPC stream
    std::shared_ptr<ASRStream::GRPCWriter> writer(
        mStub->StreamASR(ctx.get(), result.get()));

    // Store the pointers in our ASRStream object, then send the session
    // token.
    ASRStream stream(ctx, result, writer);
    if (!stream.sendToken(token))
    {
        throw ClientError(
            "failed to create ASR stream - could not send session token");
    }

    return stream;
}

TTSStream Client::newTTSStream(const cobaltspeech::diatheke::ReplyAction &reply)
{
    /*
     * Create the context. We need it to exist for the lifetime of the
     * stream, so we create it as a managed pointer. We don't set a
     * deadline on the context because we expect the stream to be long-
     * lived.
     */
    std::shared_ptr<grpc::ClientContext> ctx(new grpc::ClientContext);

    // Create the gRPC stream
    std::shared_ptr<TTSStream::GRPCReader> reader(
        mStub->StreamTTS(ctx.get(), reply));

    // Store the pointers in our TTSStream object.
    return TTSStream(ctx, reader);
}

void Client::setRequestTimeout(unsigned int milliseconds)
{
    mTimeout = milliseconds;
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

cobaltspeech::diatheke::SessionOutput
Client::updateSession(const cobaltspeech::diatheke::SessionInput &request)
{
    // Create the context
    grpc::ClientContext ctx;
    setContextDeadline(ctx);

    // Send and get a response
    cobaltspeech::diatheke::SessionOutput response;
    grpc::Status status = mStub->UpdateSession(&ctx, request, &response);
    if (!status.ok())
    {
        throw ClientError(status);
    }

    return response;
}

} // namespace Diatheke
