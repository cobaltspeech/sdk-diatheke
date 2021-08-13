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

#ifndef DIATHEKE_TRANSCRIBE_STREAM_H
#define DIATHEKE_TRANSCRIBE_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>
#include <string>

namespace Diatheke
{

class TranscribeStream
{
public:
    using GRPCReaderWriter = grpc::ClientReaderWriter<cobaltspeech::diatheke::TranscribeInput,
                                                      cobaltspeech::diatheke::TranscribeResult>;

    /*
     * Create a new Transcribe stream object using the given gRPC objects.
     * Most callers should use Client::newTranscribeStream() instead
     * of creating a new stream directly.
     */
    TranscribeStream(const std::shared_ptr<grpc::ClientContext> &ctx,
                     const std::shared_ptr<GRPCReaderWriter> &stream);
    ~TranscribeStream();

    /*
     * Send the given audio data to Diatheke for transcription.
     *
     * If this function returns false, the server has closed the stream,
     * and no further attempts should be made to send an action or audio
     * data to the server.
     *
     * It is thread-safe to call this method while also calling
     * receiveResult().
     */
    bool sendAudio(const std::string &data);

    /*
     * Send the given TranscribeAction to Diatheke to update the speech
     * recognition context. The action must first be sent on the stream
     * before any audio will be recognized. If the stream was created
     * using Client::newTranscribeStream(), this action was already sent.
     *
     * If this function returns false, the server has closed the stream,
     * and no further attempts should be made to send an action or audio
     * data to the server.
     */
    bool sendAction(const cobaltspeech::diatheke::TranscribeAction &action);

    /*
     * Tell the server that no more data will be sent over this stream.
     * It is an error to call sendAudio() or sendAction() after calling
     * this.
     */
    void sendFinished();

    /*
     * Wait for the next available TranscribeResult from the server, which
     * will be stored in the given pointer. Returns false when there are no
     * more results to receive.
     *
     * It is thread-safe to call this method while also calling
     * sendAudio().
     */
    bool receiveResult(cobaltspeech::diatheke::TranscribeResult *result);

    /*
     * Close the stream. This should be called exactly once when both:
     *
     *  - Writing is done, either because the client has no more data
     *    to send or the sendAudio() method returned false.
     *  - Reading is done, either because the client does not want to
     *    receive any more results or receiveResult() returned false.
     *
     * It is an error to use any methods of this class after calling close().
     */
    void close();

    /*
     * Provides access to the underlying gRPC stream without
     * transferring ownership of the stream.
     */
    GRPCReaderWriter *getStream();

private:
    std::shared_ptr<grpc::ClientContext> mContext;
    std::shared_ptr<GRPCReaderWriter> mStream;
};

} // namespace Diatheke

#endif // DIATHEKE_TRANSCRIBE_STREAM_H
