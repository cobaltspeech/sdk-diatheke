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

package audio

import (
	"context"
	"fmt"
	"io"
	"os/exec"

	"github.com/cobaltspeech/sdk-diatheke/examples/go/internal/config"
)

// Recorder launches an external application to handle recording audio.
type Recorder struct {
	// Internal data
	appConfig config.AudioConfig
	cmd       *exec.Cmd
	ctx       context.Context
	cancel    context.CancelFunc
	stdout    io.ReadCloser
}

// NewRecorder returns a new recorder object based the given configuration.
func NewRecorder(cfg config.AudioConfig) Recorder {
	return Recorder{
		appConfig: cfg,
	}
}

// Start the external recording application.
func (rec *Recorder) Start() error {
	if rec.cancel != nil {
		// Ignore if we are already recording
		return nil
	}

	// Create the command context so we can cancel it in the stop function.
	// This is how we can kill the external application.
	ctx, cancel := context.WithCancel(context.Background())

	// Create the record command and get its stdout pipe
	cmd := exec.CommandContext(ctx, rec.appConfig.Application,
		rec.appConfig.ArgList()...)

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		cancel()
		return err
	}

	// Run the application
	if err = cmd.Start(); err != nil {
		cancel()
		return err
	}

	// Save the command
	rec.cmd = cmd
	rec.ctx = ctx
	rec.cancel = cancel
	rec.stdout = stdout

	return nil
}

// Stop the external recording application.
func (rec *Recorder) Stop() error {
	if rec.cancel == nil || rec.cmd == nil {
		// Ignore if it is already stopped.
		return nil
	}

	// By the time we exit this function, we want everything to be reset
	defer func() {
		rec.ctx = nil
		rec.cancel = nil
		rec.cmd = nil
		rec.stdout = nil
	}()

	// Cancel the context, which should kill the executable. Then wait
	// for it to finish.
	rec.cancel()
	if err := rec.cmd.Wait(); err != nil {
		return err
	}

	return nil
}

// Read audio data from the external recording application and put it into p.
func (rec *Recorder) Read(p []byte) (n int, err error) {
	if rec.stdout == nil {
		// It is an error to call this if the recorder isn't running.
		return 0, fmt.Errorf("recorder application is not running")
	}

	// Grab data from stdout.
	return rec.stdout.Read(p)
}
