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

#ifndef PLAYER_H
#define PLAYER_H

#include <cstdio>
#include <string>
#include <vector>

// Player represents an external playback executable with args.
class Player
{
public:
    /*
     * Constructs a new player object using the given command, which
     * will be used to launch the external playback application.
     */
    Player(const std::string &playCmd);

    /*
     * Destroys the player object. If the player has not been already
     * been stopped, this will also stop the external application.
     */
    ~Player();

    /*
     * BatchPlay starts the playback application, plays all the
     * given audio, then closes the playback app. It is a convenience
     * function when working with a complete set of audio data.
     */
    void batchPlay(const char *audio, size_t len);

    // Start the external playback application.
    void start();

    // Stop the external playback application.
    void stop();

    /*
     * Push audio data to the player applicaiton. start() should
     * be called prior to using this function.
     */
    void pushAudio(const char *audio, size_t len);

private:
    std::string mCmd;
    FILE *mStdin;
};

#endif // PLAYER_H
