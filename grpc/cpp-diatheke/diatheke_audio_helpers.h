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

#ifndef DIATHEKE_AUDIO_HELPERS_H
#define DIATHEKE_AUDIO_HELPERS_H

#include "diatheke_asr_stream.h"
#include "diatheke_transcribe_stream.h"
#include "diatheke_tts_stream.h"

#include <functional>

namespace Diatheke
{

/*
 * AudioReader defines an interface for reading audio that may be
 * subclassed and used with the ReadASRAudio() and ReadTranscribeAudio()
 * methods. It is provided as a convenience.
 */
class AudioReader
{
public:
    AudioReader();
    virtual ~AudioReader();

    /*
     * Read audio data and store it in the given buffer. Returns the number
     * of bytes read, or zero if there is no more data to read.
     */
    virtual size_t readAudio(char *buffer, size_t buffSize) = 0;
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
 * ReadASRAudio is a convenience function to send audio from the given
 * reader to the stream in buffSize chunks until a result is returned.
 */
cobaltspeech::diatheke::ASRResult
ReadASRAudio(ASRStream &stream, AudioReader *reader, size_t buffSize);

/*
 * ReadTranscribeAudio is a convenience function to send audio from the given
 * reader to the stream in buffSize chunks for transcription. The results are
 * sent to the given callback function. The reader and the callback function
 * will be called from different threads, but with no guarantee that either of
 * them are run on the same thread that called this function.
 */
void ReadTranscribeAudio(TranscribeStream &stream, AudioReader *reader, size_t buffSize,
                         std::function<void(const cobaltspeech::diatheke::TranscribeResult&)> callback);

/*
 * WriteTTSAudio is a convenience function to receive audio
 * from the given TTSStream and send it to the writer until
 * there is no more audio to receive.
 */
void WriteTTSAudio(TTSStream &stream, AudioWriter *writer);

} // namespace Diatheke

#endif // DIATHEKE_AUDIO_HELPERS_H
