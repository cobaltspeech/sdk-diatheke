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
#include "diatheke_session.h"

#include <iostream>
#include <thread>

// Handle the events as they come from Diatheke.
void handleEvents(Diatheke::Session *session)
{
    // Create the event stream
    std::unique_ptr<Diatheke::EventStream> eventStream = session->eventStream();

    /*
     * Receive events from the event stream until it is closed, which will
     * happen when the session ends, or the context used to create the stream
     * closes.
     */
    cobaltspeech::diatheke::DiathekeEvent event;
    while (eventStream->waitForEvent(&event))
    {
        // Check for the event type
        if (event.has_recognize())
        {
            // Let the user know when the input was not recognized. Here we
            // don't care so much about the text that was recognized because
            // it will be the same as what the user typed on the command line.
            if (event.recognize().valid_input())
            {
                // Color the text green to highlight that it was recognized.
                std::cout << "\n\x1B[32m"
                          << "Valid input"
                          << "\x1B[0m" << std::endl;
            }
            else
            {
                // Color the text red to highlight that it was not recognized.
                std::cout << "\n\x1B[31m"
                          << "Unrecognized input"
                          << "\x1B[0m" << std::endl;
            }
        }
        else if (event.has_reply())
        {
            // Print the reply from Diatheke
            std::cout << "Reply: " << event.reply().text() << std::endl;
        }
        else if (event.has_command())
        {
            // Notify Diatheke that the command is finished. This is important
            // to do so that dialog flow may continue after the command is
            // finished.
            Diatheke::CommandStatus returnStatus(event.command());
            returnStatus.setStatusCode(Diatheke::CommandStatus::SUCCESS);
            eventStream->commandFinished(returnStatus);
        }
    }

    // It is good practice to close the event stream when we are done with it.
    eventStream->close();
}

int main(int argc, char *argv[])
{
    // Parse the config file
    DemoConfig config;
    try
    {
        config = DemoConfig::parseApplicationArgs(argc, argv);

        // The model is required for this demo
        if (config.diathekeModelID().empty())
        {
            std::cerr << "missing Diatheke ModelID in the config file"
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

    // Display the available models
    std::cout << "Available model IDs: " << std::endl;
    std::vector<std::string> models = client.listModels();
    for (const std::string &m : models)
    {
        std::cout << "  " << m << "\n";
    }
    std::cout << std::endl;

    // Create a new session
    std::string sessionID = client.newSession(config.diathekeModelID());
    Diatheke::Session session(sessionID, &client);

    // Start the event stream on a separate thread.
    std::thread eventThread(&handleEvents, &session);

    // Use stdin to get input text to send to Diatheke.
    std::cout << "Enter text at the prompt. Press Enter to send to Diatheke.\n";
    std::cout << "Use Ctrl+D to exit.\n" << std::endl;
    while (true)
    {
        // Display the prompt (color using ANSI escape codes)
        std::cout << "\x1B[94m"
                  << "Diatheke> "
                  << "\x1B[0m" << std::flush;

        // Wait for user input
        std::string userInput;
        std::getline(std::cin, userInput);
        if (std::cin.eof())
        {
            // The user selected Ctrl+D
            break;
        }

        // Send the input to Diatheke
        if (!userInput.empty())
        {
            session.pushText(userInput);
        }
    }

    // Cleanup
    std::cout << "\nExiting..." << std::endl;

    // Close the session before joining the event thread so that the
    // event stream will end first.
    session.endSession();
    eventThread.join();

    return 0;
}
