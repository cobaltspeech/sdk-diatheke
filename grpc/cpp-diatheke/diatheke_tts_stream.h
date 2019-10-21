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

#ifndef DIATHEKE_TTS_STREAM_H
#define DIATHEKE_TTS_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>

namespace Diatheke
{

class TTSStream
{
public:
    using Reader = grpc::ClientReader<cobaltspeech::diatheke::TTSResponse>;

    /*
     * Wrap the given Reader and context in a new TTSStream object.
     * Most users will not need to call this constructor directly, but should
     * instead use Diatheke::Client::streamTTS().
     */
    TTSStream(const std::shared_ptr<Reader> &stream,
              const std::shared_ptr<grpc::ClientContext> &ctx);

    ~TTSStream();

    /*
     * Wait for the next ASR result from the server to become available.
     * Returns false when there are no more results to receive. It is thread-
     * safe to call this method while also calling pushAudio().
     */
    bool waitForAudio(cobaltspeech::diatheke::TTSResponse *response);

    /*
     * Close the TTS stream. This should be done after waitForAudio is
     * finished (i.e., returned false).
     */
    void close();

private:
    std::shared_ptr<Reader> mStream;
    std::shared_ptr<grpc::ClientContext> mContext;

    // Disable copy construction and assignments
    TTSStream(const TTSStream &other);
    TTSStream &operator=(const TTSStream &other);
};

} // namespace Diatheke

#endif // DIATHEKE_TTS_STREAM_H
