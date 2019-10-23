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

/*
 * Define ANSI escape codes that we use to polish the terminal-based UI
 * in the demos. See https://en.wikipedia.org/wiki/ANSI_escape_code for
 * an introduction to these escape codes.
 */

// This resets the text color to its default state.
const char *ANSIResetText = "\x1B[0m";

// Set the text to red
const char *ANSIRedText = "\x1B[31m";

// Set the text to green
const char *ANSIGreenText = "\x1B[32m";

/*
 * This is labelled white, but the actual color will depend on the terminal.
 * For terminals with a white background, it seems to be grey.
 */
const char *ANSIWhiteText = "\x1B[37m";

// Set the text to bright blue
const char *ANSIBrightBlueText = "\x1B[94m";
