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

#include "demo_config.h"
#include "diatheke_client.h"
#include "player.h"

int main(int argc, char *argv[])
{
    // Parse the config file
    DemoConfig config;
    try
    {
        config = DemoConfig::parseApplicationArgs(argc, argv);

        // The following is required for this demo
        if (config.playbackCmd().empty())
        {
            std::cerr << "missing Playback Application in the config file"
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
    std::cout << "Connected to " << config.diathekeServerAddress() << "\n"
              << std::endl;

    // Prompt the user for the Cubic model
    std::cout << "Please enter the Luna voice model ID: " << std::flush;
    std::string lunaModel;
    std::getline(std::cin, lunaModel);
    std::cout << std::endl;

    // Start the main loop
    std::cout << "Enter text to synthesize at the prompt.\n";
    std::cout << "Use Ctrl+D to exit.\n" << std::endl;
    while (true)
    {
        // Display the prompt (color using ANSI escape codes)
        std::cout << "\x1B[94m"
                  << "Diatheke TTS> "
                  << "\x1B[0m" << std::flush;

        // Wait for user input
        std::string userInput;
        std::getline(std::cin, userInput);
        if (std::cin.eof())
        {
            // The user selected Ctrl+D
            break;
        }

        if (userInput.empty())
        {
            // Don't bother synthesizing if there is no text.
            continue;
        }

        std::cout << "Synthesizing using voice model \"" << lunaModel << "\"..."
                  << std::endl;

        // Start the player application
        Player player(config.playbackCmd());
        player.start();

        // Use the text to run synthesis
        std::unique_ptr<Diatheke::TTSStream> stream =
            client.streamTTS(lunaModel, userInput);

        cobaltspeech::diatheke::TTSResponse response;
        while (stream->waitForAudio(&response))
        {
            player.pushAudio(response.data().c_str(), response.data().size());
        }

        // Cleanup the stream and player.
        stream->close();
        player.stop();
        std::cout << "Synthesis complete.\n" << std::endl;
    }

    std::cout << "\nExiting..." << std::endl;

    return 0;
}
