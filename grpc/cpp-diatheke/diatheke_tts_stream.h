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

#ifndef DIATHEKE_TTS_STREAM_H
#define DIATHEKE_TTS_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>

namespace Diatheke
{

class TTSStream
{
public:
    using GRPCReader = grpc::ClientReader<cobaltspeech::diatheke::TTSAudio>;

    /*
     * Create a new TTSStream object using the given gRPC objects.
     * Most callers should use Client::newTTSStream() instead of
     * creating the stream directly.
     */
    TTSStream(const std::shared_ptr<grpc::ClientContext> &ctx,
              const std::shared_ptr<GRPCReader> &stream);

    ~TTSStream();

    /*
     * Wait for the next chunk of audio data from the server,
     * and stores the audio data in the given buffer. Returns
     * false when there is no more audio to receive.
     */
    bool receiveAudio(std::string &buffer);

    /*
     * Provides access to the underlying gRPC stream without
     * transferring ownership of the stream.
     */
    GRPCReader *getStream();

private:
    bool mClosed;
    std::shared_ptr<grpc::ClientContext> mContext;
    std::shared_ptr<GRPCReader> mStream;
};

/*
 * AudioWriter defines an interface for writing audio data that may
 * be subclassed and used with the WriteTTSAudio() method. It is
 * provided as a convenience.
 */
class AudioWriter
{
public:
    AudioWriter();
    virtual ~AudioWriter();

    /*
     * Write audio data from the given buffer. Returns the number
     * of bytes written.
     */
    virtual size_t writeAudio(const char *buffer, size_t sizeInBytes) = 0;
};

/*
 * WriteTTSAudio is a convenience function to receive audio
 * from the given TTSStream and send it to the writer until
 * there is no more audio to receive.
 */
void WriteTTSAudio(TTSStream &stream, AudioWriter *writer);

} // namespace Diatheke

#endif // DIATHEKE_TTS_STREAM_H
