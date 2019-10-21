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

#ifndef DEMO_CONFIG_H
#define DEMO_CONFIG_H

#include <string>

class DemoConfig
{
public:
    /*
     * Parse the command-line arguments given to application at startup
     * and return the configuration based on the args. It is expecting
     * "-config <path/to/config.toml>" to be included in the arguments and
     * throws an exception if they are not (or if there are errors parsing).
     */
    static DemoConfig parseApplicationArgs(int argc, char *argv[]);

    /*
     * Parse the given configuration file and return a DemoConfig object.
     * Throws an exception if errors are encountered.
     */
    static DemoConfig parseFile(const std::string &filename);

    DemoConfig();
    ~DemoConfig();

    /*
     * Returns true if an insecure connection should be made with Diatheke
     * server.
     */
    bool diathekeServerInsecure() const;

    // Returns the url of the Diatheke server (with port number)
    const std::string &diathekeServerAddress() const;

    // Returns the ID of the Diatheke model to use when creating a new session.
    const std::string &diathekeModelID() const;

    // Returns the record command (with args)
    const std::string &recordCmd() const;

    // Returns the playback command (with args)
    const std::string &playbackCmd() const;

private:
    bool mInsecure;
    std::string mServer;
    std::string mModelID;
    std::string mRecord;
    std::string mPlayback;
};

#endif // DEMO_CONFIG_H
