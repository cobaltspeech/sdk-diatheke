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

#ifndef DIATHEKE_ASR_STREAM_H
#define DIATHEKE_ASR_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>

namespace Diatheke
{

class ASRStream
{
public:
    using ReaderWriter =
        grpc::ClientReaderWriter<cobaltspeech::diatheke::ASRRequest,
                                 cobaltspeech::diatheke::ASRResponse>;

    /*
     * Wrap the given Reader/Writer and context in a new ASRStream object.
     * Most users will not need to call this constructor directly, but should
     * instead use Diatheke::Client::streamASR().
     */
    ASRStream(const std::shared_ptr<ReaderWriter> &stream,
              const std::shared_ptr<grpc::ClientContext> &ctx);

    ~ASRStream();

    /*
     * Push the given audio data to the Diatheke server. The audio format
     * should match what is specified in the Diatheke server configuration.
     */
    void pushAudio(const char *audio, size_t sizeInBytes);

    /*
     * Notify Diathake that no more audio is coming from the client. It is
     * an error to call pushAudio() after calling this method.
     */
    void finishAudio();

    /*
     * Wait for the next ASR result from the server to become available.
     * Returns false when there are no more results to receive. It is thread-
     * safe to call this method while also calling pushAudio().
     */
    bool waitForResult(cobaltspeech::diatheke::ASRResponse *result);

    /*
     * Close the ASR stream. This should be done after both pushAudio and
     * waitForResult are finished.
     */
    void close();

private:
    std::shared_ptr<ReaderWriter> mStream;
    std::shared_ptr<grpc::ClientContext> mContext;

    // Disable copy construction and assignments
    ASRStream(const ASRStream &other);
    ASRStream &operator=(const ASRStream &other);
};

} // namespace Diatheke

#endif // DIATHEKE_ASR_STREAM_H
