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
#include "player.h"
#include "recorder.h"

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

// Records audio from the external recording app and sends it to Diatheke.
void recordAudio(Diatheke::Session *session, std::string recordCmd,
                 std::atomic_bool &isRecording)
{
    /*
     * Create the audio input stream. This will fail if such a stream already
     * exists for the given session.
     */
    std::unique_ptr<Diatheke::AudioInputStream> audioInput =
        session->streamAudioInput();

    // Set up the recording application.
    Recorder rec(recordCmd);
    rec.start();

    // Push audio to the stream until we are done recording.
    while (isRecording)
    {
        std::string data = rec.readAudio(8192);
        audioInput->pushAudio(data.c_str(), data.size());
    }

    // Stop the recording app and let Diatheke know we are done.
    rec.stop();
    audioInput->finished();
}

/*
 * Handles audio replies from Diatheke, forwarding the audio data to the
 * external playback app.
 */
void handleAudioReplies(Diatheke::Session *session, std::string playCmd)
{
    // Setup the playback application
    Player player(playCmd);

    // Create the audio reply stream
    std::unique_ptr<Diatheke::AudioReplyStream> audioReplies =
        session->streamAudioReplies();

    /*
     * Wait for replies until the stream is closed, which will happen when
     * the session is closed or the stream's context stops.
     */
    cobaltspeech::diatheke::AudioReply reply;
    while (audioReplies->waitForReply(&reply))
    {
        // Check which message we have received
        switch (reply.output_message_case())
        {
        case cobaltspeech::diatheke::AudioReply::kText:
            /*
             * We received the text of the reply, which comes at the beginning.
             * Start the playback application so it is ready to play audio.
             */
            player.start();
            break;

        case cobaltspeech::diatheke::AudioReply::kData:
            /*
             * We receive this until speech synthesis is done. Send the
             * binary data to the playback application. The format of the
             * data is specified in the Diatheke server configuration.
             */
            player.pushAudio(reply.data().c_str(), reply.data().size());
            break;

        case cobaltspeech::diatheke::AudioReply::kEnd:
            /*
             * We receive this at the end of a reply, after speech synthesis
             * is complete. Stop the playback application.
             */
            player.stop();
            break;

        default:
            std::cerr << "received unexpected message in AudioReply stream"
                      << std::endl;
            break;
        }
    }

    audioReplies->close();
}

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
            if (event.recognize().valid_input())
            {
                // Color the text green if it is valid
                std::cout << "\x1B[32m";
            }
            else
            {
                // Color the text red if it is not recognized as valid
                std::cout << "\x1B[31m";
            }

            // Print out the text and reset the color
            std::cout << "Me: " << event.recognize().text() << "\x1B[0m"
                      << std::endl;
        }
        else if (event.has_reply())
        {
            std::cout << "  \x1B[94mDiatheke: " << event.reply().text()
                      << "\x1B[0m"
                      << "\n"
                      << std::endl;
        }
        else
        {
            // TODO: something awesome here with the command
            if (false)
            {
                /*
                 * This section demonstrates how to get parameters from the
                 * command event. To avoid cluttering the output, it is
                 * currently hidden. To view it, change the if statement to
                 * true.
                 */
                std::cout << "    Command ID: " << event.command().command_id()
                          << std::endl;
                std::cout << "    Parameters:\n";
                for (const auto &pair : event.command().parameters())
                {
                    std::cout << "      " << pair.first << " " << pair.second
                              << std::endl;
                }
                std::cout << std::flush;
            }

            // Notify Diatheke that the command is finished.
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

        // The following are required for this demo
        if (config.diathekeModelID().empty())
        {
            std::cerr << "missing Diatheke ModelID in the config file"
                      << std::endl;
            return 1;
        }

        if (config.recordCmd().empty())
        {
            std::cerr << "missing Recording Application in the config file"
                      << std::endl;
            return 1;
        }

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

    // Start audio reply streaming on another thread.
    std::thread replyThread(&handleAudioReplies, &session,
                            config.playbackCmd());

    // Use stdin to toggle recording
    std::cout << "Use Enter to start/stop recording.\n"
              << "Use Ctrl+D to exit.\n"
              << std::endl;
    std::atomic_bool isRecording(false);
    std::thread *recordThread(nullptr);
    while (true)
    {
        std::cout << "\x1B[37m";
        if (isRecording)
        {
            std::cout << "(Recording)";
        }
        else
        {
            std::cout << "(Waiting to record)";
        }
        std::cout << "\x1B[0m" << std::endl;

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
            // Start the audio recording thread
            recordThread =
                new std::thread(&recordAudio, &session, config.recordCmd(),
                                std::ref(isRecording));
        }
        else
        {
            // Stop the audio recording thread
            recordThread->join();
            delete recordThread;
            recordThread = nullptr;
        }
    }

    // Cleanup
    std::cout << "\nExiting..." << std::endl;
    if (recordThread)
    {
        isRecording = false;
        recordThread->join();
        delete recordThread;
    }

    // Close the session before joining threads so that each thread's
    // stream will end first.
    session.endSession();
    eventThread.join();
    replyThread.join();

    return 0;
}
