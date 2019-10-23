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

#include "ansi_escape_codes.h"
#include "demo_config.h"
#include "diatheke_client.h"
#include "recorder.h"

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

// Records audio from the external recording app and sends it to Diatheke.
void recordAudio(std::shared_ptr<Diatheke::ASRStream> stream,
                 std::string recordCmd, std::atomic_bool &isRecording)
{
    // Set up the recording application
    Recorder rec(recordCmd);
    rec.start();

    // Push audio to the stream until we are done recording
    while (isRecording)
    {
        // Read audio data from the recorder
        std::string data = rec.readAudio();
        stream->pushAudio(data.c_str(), data.size());
    }

    /*
     * Stop the recording app and let Diatheke we will not be sending more
     * audio over this stream.
     */
    rec.stop();
    stream->finishAudio();
}

// Handle ASR results as they come back from Diatheke
void handleResults(std::shared_ptr<Diatheke::ASRStream> stream)
{
    // Wait for a response from the server until there are no more.
    cobaltspeech::diatheke::ASRResponse result;
    while (stream->waitForResult(&result))
    {
        std::cout << "ASR Response:" << std::endl;
        std::cout << "  Transcription: " << result.text() << std::endl;
        std::cout << "  Confidence Score: " << result.confidence_score()
                  << std::endl
                  << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // Parse the config file
    DemoConfig config;
    try
    {
        config = DemoConfig::parseApplicationArgs(argc, argv);

        // The following is required for this demo
        if (config.recordCmd().empty())
        {
            std::cerr << "missing Recording Application in the config file"
                      << std::endl;
            return 1;
        }
    }
    catch (const std::exception &err)
    {
        std::cerr << "error parsing config file: " << err.what() << std::endl;
        return 1;
    }

    // Create the client
    Diatheke::Client client(config.diathekeServerAddress(),
                            config.diathekeServerInsecure());

    // Display the diatheke version
    std::string version = client.diathekeVersion();
    std::cout << "Diatheke version: " << version << std::endl;
    std::cout << "Connected to " << config.diathekeServerAddress() << std::endl;
    std::cout << std::endl;

    // Prompt the user for the Cubic model
    std::cout << "Please enter the Cubic model ID: " << std::flush;
    std::string cubicModel;
    std::getline(std::cin, cubicModel);
    std::cout << std::endl;

    // Use stdin to toggle recording
    std::cout << "Use Enter to start/stop recording." << std::endl;
    std::cout << "Use Ctrl+D to exit." << std::endl << std::endl;

    std::atomic_bool isRecording(false);
    std::unique_ptr<std::thread> recordThread;
    std::unique_ptr<std::thread> resultThread;
    std::shared_ptr<Diatheke::ASRStream> stream;
    while (true)
    {
        if (isRecording == false)
        {
            std::cout << ANSIWhiteText << "(Press Enter to record)"
                      << ANSIResetText << std::endl;
        }

        // Wait for the user to press Enter
        std::string userInput;
        std::getline(std::cin, userInput);
        if (std::cin.eof())
        {
            // The user selected Ctrl+D
            break;
        }

        // Toggle the recording state
        isRecording = !isRecording;
        if (isRecording)
        {
            /*
             * Create the ASR stream independent of any running session. This
             * is useful for general ASR tasks that are unrelated to a
             * conversation.
             */
            std::cout << "Running ASR with model \"" << cubicModel << "\"..."
                      << std::endl;
            stream = client.streamASR(cubicModel);

            // Start the recording and result threads
            recordThread.reset(new std::thread(&recordAudio, stream,
                                               config.recordCmd(),
                                               std::ref(isRecording)));

            resultThread.reset(new std::thread(&handleResults, stream));
        }
        else
        {
            // Wait for the threads to finish and close the stream.
            recordThread->join();
            recordThread.reset();

            resultThread->join();
            resultThread.reset();

            stream->close();
            stream.reset();
        }
    }

    // Cleanup
    std::cout << std::endl << "Exiting..." << std::endl;
    if (recordThread)
    {
        isRecording = false;
        recordThread->join();
        recordThread.reset();
    }

    if (resultThread)
    {
        resultThread->join();
        resultThread.reset();
    }

    if (stream)
    {
        stream->close();
        stream.reset();
    }

    return 0;
}
