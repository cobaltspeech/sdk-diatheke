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
	"os"
)

// Define ANSI escape codes for text coloring. See
// https://en.wikipedia.org/wiki/ANSI_escape_code for an introduction
// to these escape codes.
const (
	ansiReset      = "\x1B[0m"
	ansiRed        = "\x1B[31m"
	ansiGreen      = "\x1B[32m"
	ansiWhite      = "\x1B[37m"
	ansiBrightBlue = "\x1B[94m"
)

// PrintError writes the given message to stderr
func PrintError(msg string, a ...interface{}) {
	fmt.Fprintf(os.Stderr, msg, a...)
}

// PrintGreen wraps the fmt.Printf function with ANSI escape codes
// to color the given text green.
func PrintGreen(format string, a ...interface{}) {
	printColor(ansiGreen, format, a...)
}

// PrintRed wraps the fmt.Printf function with ANSI escape codes to
// color the given text red.
func PrintRed(format string, a ...interface{}) {
	printColor(ansiRed, format, a...)
}

// PrintWhite wraps the fmt.Printf function with ANSI escape codes to
// color the given text white.
func PrintWhite(format string, a ...interface{}) {
	printColor(ansiWhite, format, a...)
}

// PrintBlue wraps the fmt.Printf function with ANSI escape codes to
// color the given text blue.
func PrintBlue(format string, a ...interface{}) {
	printColor(ansiBrightBlue, format, a...)
}

func printColor(color, format string, a ...interface{}) {
	os.Stdout.WriteString(color)
	fmt.Printf(format, a...)
	os.Stdout.WriteString(ansiReset)
}
