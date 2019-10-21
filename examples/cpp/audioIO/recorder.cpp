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

Recorder::Recorder(const std::string &record_cmd)
    : mCmd(record_cmd), mStdout(nullptr)
{
}

Recorder::~Recorder() {}

void Recorder::start()
{
    if (mStdout)
        return;

    // Start the external process
    mStdout = popen(mCmd.c_str(), "r");
}

std::string Recorder::readAudio(size_t maxBufferSize)
{
    if (!mStdout)
        return "";

    char *buffer = new char[maxBufferSize];

    size_t charsRead = fread(buffer, 1, maxBufferSize, mStdout);

    std::string result(buffer, charsRead);
    delete[] buffer;
    return result;
}

void Recorder::stop()
{
    // Close the stdout stream, which should also close the application.
    pclose(mStdout);
    mStdout = nullptr;
}
