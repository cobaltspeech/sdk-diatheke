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

#include <cpptoml.h>
#include <stdexcept>

DemoConfig DemoConfig::parseApplicationArgs(int argc, char *argv[])
{
    std::string configFilename;

    if (argc == 1)
    {
        // No arguments were specified. Use a default config filename.
        configFilename = "config.toml";
    }

    // Look for the -config option
    for (int i = 0; i < argc; i++)
    {
        // Check if this is the config arg
        if (strcmp(argv[i], "-config") == 0 || strcmp(argv[i], "--config") == 0)
        {
            // Check that we also have enough args to include the filename
            if (i + 1 < argc)
            {
                configFilename.assign(argv[i + 1]);
                break;
            }
        }
    }

    if (configFilename.empty())
    {
        throw std::runtime_error(
            "missing config file.\n"
            "Please specify the configuration file using the \"-config "
            "<path/to/config.toml>\" option.");
    }

    return parseFile(configFilename);
}

DemoConfig DemoConfig::parseFile(const std::string &filename)
{
    DemoConfig config;

    // Parse the toml file
    std::shared_ptr<cpptoml::table> toml = cpptoml::parse_file(filename);

    // Check for the insecure connection (defaults to false)
    config.mInsecure =
        toml->get_qualified_as<bool>("Server.Insecure").value_or(false);

    // Check for the server address (required)
    config.mServer = *(toml->get_qualified_as<std::string>("Server.Address"));
    if (config.mServer.empty())
    {
        throw std::runtime_error("required field Server.Address missing");
    }

    // Get the Diatheke model to use
    config.mModelID = *(toml->get_qualified_as<std::string>("Server.ModelID"));

    // Get the Record command.
    std::string recordApp, recordArgs;
    recordApp = *(toml->get_qualified_as<std::string>("Recording.Application"));
    recordArgs = *(toml->get_qualified_as<std::string>("Recording.Args"));
    config.mRecord = recordApp;
    if (recordArgs.empty() == false)
    {
        config.mRecord += " " + recordArgs;
    }

    // Get the Playback command.
    std::string playApp, playArgs;
    playApp = *(toml->get_qualified_as<std::string>("Playback.Application"));
    playArgs = *(toml->get_qualified_as<std::string>("Playback.Args"));
    config.mPlayback = playApp;
    if (playArgs.empty() == false)
    {
        config.mPlayback += " " + playArgs;
    }

    return config;
}

DemoConfig::DemoConfig() {}

DemoConfig::~DemoConfig() {}

bool DemoConfig::diathekeServerInsecure() const { return mInsecure; }

const std::string &DemoConfig::diathekeServerAddress() const { return mServer; }

const std::string &DemoConfig::diathekeModelID() const { return mModelID; }

const std::string &DemoConfig::recordCmd() const { return mRecord; }

const std::string &DemoConfig::playbackCmd() const { return mPlayback; }
