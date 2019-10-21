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
    Recorder(const std::string &record_cmd);
    ~Recorder();

    // Start recording audio.
    void start();

    // Read audio data from the recorder up to the maxBufferSize
    // (in bytes). The binary data is returned as a string.
    std::string readAudio(size_t maxBufferSize);

    // Stop recording audio, and return the recorded data.
    void stop();

private:
    std::string mCmd;
    FILE *mStdout;
};

#endif // RECORDER_H
