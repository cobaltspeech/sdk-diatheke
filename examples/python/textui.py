# -*- coding: utf-8 -*-
#
# Copyright(2019) Cobalt Speech and Language Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http: // www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# Define ANSI escape codes for text coloring. See
# https://en.wikipedia.org/wiki/ANSI_escape_code for an introduction
# to these escape codes.
class ANSI:
    reset = "\x1B[0m"
    red = "\x1B[31m"
    green = "\x1B[32m"
    white = "\x1B[37m"
    bright_blue = "\x1B[94m"


def print_color(color, text):
    """Print the text with the specified color. Color should be an ANSI
    escape code."""
    print(color + text + ANSI.reset)


def print_red(text):
    """Print the text using red."""
    print_color(ANSI.red, text)


def print_green(text):
    """Print the text using green."""
    print_color(ANSI.green, text)


def print_white(text):
    """Print the text using white."""
    print_color(ANSI.white, text)


def print_blue(text):
    """Print the text using blue."""
    print_color(ANSI.bright_blue, text)


class PromptUI(object):
    """PromptUI is a text UI that displays a custom prompt and runs until
    the user presses Ctrl+D.
    """

    def __init__(self, prompt, text_callback):
        """
        :param prompt: The text for the prompt (e.g., "Diatheke> ")
        :param text_callback: Function that is called when the user has
                              finished providing input (i.e., Enter was
                              pressed). The function should accept a string
                              (the user-input text) as a parameter.
        """
        self.prompt = prompt
        self.text_func = text_callback

    def run(self):
        """Run the PromptUI until the user exits either cleanly (with Ctrl+D) or
        with an interrupt.
        """
        print_white("(Enter text at the prompt. Use Ctrl+D to exit)\n")

        # Run the main loop until the user pressed Ctrl+D
        while True:
            try:
                # Wait for user input (color the prompt)
                text = input(ANSI.bright_blue + self.prompt + ANSI.reset)

                # Send the text to the provided callback
                self.text_func(text)

            except EOFError:
                break

        print("\nExiting...")


class ToggleUI(object):
    """ToggleUI is a text UI that toggles between two states when the user
    presses the Enter key.
    """

    def __init__(self, on_prompt, off_prompt, toggle_callback):
        """
        :param on_prompt: The prompt to display in the ON state
        :param off_prompt: The prompt to display in the OFF state
        :param toggle_callback: Function that is called when the toggle state
                                changes. This function should accept a bool as
                                a parameter.
        """
        self.on_prompt = on_prompt
        self.off_prompt = off_prompt
        self.toggle_func = toggle_callback
        self.state = False

    def run(self):
        """Run the ToggleUI until the user exits either cleanly (with Ctrl+D) or
        with an interrupt.
        """
        # Display the first prompt
        print_white(self.off_prompt)

        # Run the main loop until the user presses Ctrl+D
        while True:
            try:
                # Wait for the user to press enter. We don't care if
                # they have other input on the line.
                input()

                # Toggle the current state
                self.state = not self.state

                # Run the callback before we print the prompt (the prompt is
                # supposed to indicate the application is in a state that is
                # ready for user action).
                self.toggle_func(self.state)

                # Print the appropriate prompt
                if self.state:
                    print_white(self.on_prompt)
                else:
                    print_white(self.off_prompt)

            except EOFError:
                # This indicates the user pressed Ctrl+D
                break

        print("\nExiting...")
