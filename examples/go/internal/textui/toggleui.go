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

import "github.com/c-bata/go-prompt"

// ToggleExecutor defines methods to get prompt text and execute
// functions as the user toggles between on/off states.
type ToggleExecutor interface {
	// GetPrompt returns a string for the given toggle state.
	GetPrompt(toggleState bool) string

	// ToggleOn runs code for the on state.
	ToggleOn()

	// ToggleOff runs code for the off state.
	ToggleOff()
}

// RunToggleCLI runs a basic on/off state user interface. The user
// presses Enter to toggle between states, which will cause the given
// ToggleExecutor to run.
func RunToggleCLI(exec ToggleExecutor) {
	// Create our toggle variable
	toggled := false

	// Display the first prompt
	printTogglePrompt(toggled, exec)

	// Create the user-input callback, which in this case will handle
	// toggling between states and calling the ToggleExecutor for the
	// new state.
	cb := func(string) {
		// We don't care what the text is that the user has entered. We
		// only care that this function was called, which means the user
		// pressed Enter.
		toggled = !toggled

		// Print the prompt
		printTogglePrompt(toggled, exec)

		// Run the interface
		if toggled {
			exec.ToggleOn()
		} else {
			exec.ToggleOff()
		}
	}

	// Create the prompt
	p := prompt.New(
		cb,

		// The auto-completer does nothing
		func(prompt.Document) []prompt.Suggest { return nil },

		// Set the prompt text to empty. We will manually handle
		// printing prompts through the ToggleExecutor interface.
		prompt.OptionPrefix(""),
	)

	// Run the prompt until the user terminates with Ctrl+D
	p.Run()
}

func printTogglePrompt(toggled bool, exec ToggleExecutor) {
	promptText := exec.GetPrompt(toggled)
	PrintWhite(promptText + "\n")
}
