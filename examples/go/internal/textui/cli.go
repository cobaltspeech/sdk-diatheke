// Copyright (2019) Cobalt Speech and Language Inc.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package textui

import (
	"fmt"

	"github.com/c-bata/go-prompt"
)

// CLI creates a command-line interface that prompts a user for input.
type CLI struct {
	PromptText string
	Callback   func(text string)
}

// Run the CLI until the user presses Ctrl+D to exit.
func (cli *CLI) Run() {

	fmt.Printf("\nEnter text at the prompt. Use Ctrl+D to exit.\n\n")

	// Create the prompt
	p := prompt.New(
		// The executor simply calls our callback
		func(text string) { cli.Callback(text) },

		// The auto-completer does nothing
		func(prompt.Document) []prompt.Suggest { return nil },

		// Set the prompt text and color
		prompt.OptionPrefix(cli.PromptText),
		prompt.OptionPrefixTextColor(prompt.Blue),
	)

	// Run the prompt until the user terminates with Ctrl+D
	p.Run()
}

// SimplePrompt returns the user input for a single prompt
func SimplePrompt(promptText string) string {
	return prompt.Input(
		promptText,

		// The auto-completer does nothing
		func(prompt.Document) []prompt.Suggest { return nil },

		// Set the prompt color
		prompt.OptionPrefixTextColor(prompt.Blue),
	)
}
