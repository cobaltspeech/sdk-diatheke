package main

import (
	"context"
	"io"
	"os/exec"
)

// Recorder represents an external application capable of
// recording audio from a device and streaming it to stdout.
type Recorder struct {
	ExeName string
	ExeArgs []string

	// Internal data
	ctx       context.Context
	ctxCancel context.CancelFunc
	cmd       *exec.Cmd
}

// Start running the recorder, returning the output of the recorder
// for reading (i.e., the audio data).
func (r *Recorder) Start() (io.Reader, error) {
	// Stop it if we already have the external app running.
	if r.ctxCancel != nil {
		r.Stop()
	}

	// Create the command context so we can cancel it in the
	// stop function.
	r.ctx, r.ctxCancel = context.WithCancel(context.Background())

	// Create the record command
	r.cmd = exec.CommandContext(r.ctx, r.ExeName, r.ExeArgs...)

	// Get the stdout of the command
	stdout, err := r.cmd.StdoutPipe()
	if err != nil {
		return nil, err
	}

	// Run the command
	if err = r.cmd.Start(); err != nil {
		return nil, err
	}

	return stdout, nil
}

// Stop the audio recorder.
func (r *Recorder) Stop() error {
	if r.ctxCancel == nil {
		return nil
	}

	if r.cmd == nil {
		return nil
	}

	// Cancel the context, which should kill the executable.
	// Then wait for it to finish.
	defer func() {
		// Make sure we set these back to nil when the function
		// exits.
		r.ctxCancel = nil
		r.cmd = nil
	}()

	r.ctxCancel()
	if err := r.cmd.Wait(); err != nil {
		return err
	}

	return nil
}

// Playback the given audio data with the user-specified application
func Playback(data []byte, playConfig AudioConfig) error {

	cmd := exec.Command(playConfig.Application, playConfig.ArgList()...)
	stdin, err := cmd.StdinPipe()
	if err != nil {
		return err
	}

	// Begin writing the samples to stdin on a different thread
	go func() {
		// Close stdin when we are done writing, which should also close
		// the player external app.
		defer stdin.Close()
		stdin.Write(data)
	}()

	// Run the command and wait for it's completion.
	if err := cmd.Run(); err != nil {
		return err
	}

	return nil
}
