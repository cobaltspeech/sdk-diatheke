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

#ifndef DIATHEKE_AUDIO_REPLY_STREAM_H
#define DIATHEKE_AUDIO_REPLY_STREAM_H

#include "diatheke.grpc.pb.h"

#include <memory>
#include <string>

namespace Diatheke
{

class AudioReplyStream
{
public:
    using AudioReplyReader =
        grpc::ClientReader<cobaltspeech::diatheke::AudioReply>;

    /*
     * Wrap the given reader and context in a new AudioReplyStream object.
     * Most users will not need to call this constructor directly, but
     * should instead use Diatheke::Client::streamAudioReplies() or
     * Diatheke::Session::streamAudioReplies().
     */
    AudioReplyStream(const std::shared_ptr<AudioReplyReader> &reader,
                     const std::shared_ptr<grpc::ClientContext> &ctx,
                     const std::string &parentSessionID);

    ~AudioReplyStream();

    // Returns the ID of the parent session for this stream.
    std::string sessionID();

    /*
     * Wait to receive Diatheke reply data from the server. Returns false when
     * there are no more replies to receive (i.e., when the session ends).
     *
     * For any given reply from Diatheke, this will first receive data
     * indicating what the original text of the reply was. Then, it will receive
     * one or more messages containing audio data. Lastly, it will receive an
     * empty message to indicate the audio synthesis for the original text is
     * complete.
     */
    bool waitForReply(cobaltspeech::diatheke::AudioReply *data);

    /*
     * Close the reply stream. This should be done after all replies have
     * been received (i.e., waitForReply() returned false).
     */
    void close();

private:
    std::shared_ptr<AudioReplyReader> mStream;
    std::shared_ptr<grpc::ClientContext> mContext;
    std::string mSessionID;
};

} // namespace Diatheke

#endif // DIATHEKE_AUDIO_REPLY_STREAM_H
