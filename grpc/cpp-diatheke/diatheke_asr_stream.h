/*
 * Copyright (2021-present) Cobalt Speech and Language, Inc. All rights reserved.
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

#ifndef DIATHEKE_ASR_STREAM_H
#define DIATHEKE_ASR_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>
#include <string>

namespace Diatheke
{

class ASRStreamPrivate;

class ASRStream
{
public:
    using GRPCWriter = grpc::ClientWriter<cobaltspeech::diatheke::ASRInput>;

    /*
     * Create a new ASR stream object using the given gRPC objects.
     * Most callers should use Client::newSessionASRStream() instead
     * of creating a new stream directly.
     */
    ASRStream(cobaltspeech::diatheke::Diatheke::Stub *stub);
    ~ASRStream();

    /*
     * Send the given audio data to Diatheke for transcription.
     *
     * If this function returns False, the server has closed the stream
     * and result() should be called to get the final ASR result.
     */
    bool sendAudio(const std::string &data);

    /*
     * Send the given session token to Diatheke to update the speech
     * recognition context. The session token must first be sent on the
     * ASR stream before any audio will be recognized. If the stream was
     * created using Client::newSessionASRStream(), the first token was
     * already sent.
     *
     * If this function returns false, the server has closed the stream
     * and result() should be called to get the final ASR result.
     */
    bool sendToken(const cobaltspeech::diatheke::TokenData &token);

    /*
     * Returns the result of speech recognition. This function may be
     * called to end the audio stream early, which will force a
     * transcription based on the audio received until this point. This
     * may also be called in response to receiving false from sendAudio()
     * or sendToken(), in which case the server already has a transcription
     * ready.
     *
     * Once this method is called, data can no longer be sent over the
     * stream.
     */
    cobaltspeech::diatheke::ASRResult result();

    /*
     * Provides access to the underlying gRPC stream without
     * transferring ownership of the stream.
     */
    GRPCWriter *getStream();

private:
    std::shared_ptr<ASRStreamPrivate> dPtr; // Opaque pointer
};

} // namespace Diatheke

#endif // DIATHEKE_ASR_STREAM_H
