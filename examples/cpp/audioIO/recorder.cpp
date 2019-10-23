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

#include "recorder.h"

#include <unistd.h>

Recorder::Recorder(const std::string &record_cmd, size_t maxBuffSize)
    : mCmd(record_cmd), mBufferSize(maxBuffSize), mStdout(nullptr)
{
}

Recorder::~Recorder() {}

void Recorder::start()
{
    // Ignore if the recorder is already running
    if (mStdout)
        return;

    // Start the external process
    mStdout = popen(mCmd.c_str(), "r");
}

std::string Recorder::readAudio()
{
    // Throw an error if the recorder is not running.
    if (mStdout == nullptr)
    {
        throw std::runtime_error("can't read audio - recorder not started.");
    }

    char *buffer = new char[mBufferSize];

    size_t charsRead = fread(buffer, 1, mBufferSize, mStdout);

    std::string result(buffer, charsRead);
    delete[] buffer;
    return result;
}

void Recorder::stop()
{
    // Ignore if the recorder is already stopped
    if (mStdout == nullptr)
    {
        return;
    }

    // Close the stdout stream, which should also close the application.
    pclose(mStdout);
    mStdout = nullptr;
}
