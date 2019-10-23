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

#ifndef RECORDER_H
#define RECORDER_H

#include <cstdio>
#include <string>

class Recorder
{
public:
    /*
     * Create a new recorder instance that will launch the given external
     * application (record_cmd). maxBuffSize defines the maximum amount of
     * audio data (in bytes) that will be retrieved with each call to
     * readAudio(). The default is 8kB.
     */
    Recorder(const std::string &record_cmd, size_t maxBuffSize = 8192);
    ~Recorder();

    // Start recording audio.
    void start();

    /*
     * Read audio data from the recorder app. The binary data is returned
     * as a string (think of it as an array of chars, or bytes).
     */
    std::string readAudio();

    // Stop recording audio, and return the recorded data.
    void stop();

private:
    std::string mCmd;
    size_t mBufferSize;
    FILE *mStdout;
};

#endif // RECORDER_H
