package main

import (
	"context"
	"io"
	"os/exec"
	"sync"
)

// AudioIO manages the audio input/output using external
// applications. It also handles automatically muting the
// recorder during playback.
type AudioIO struct {
	// Internal data
	recorderApp  string
	recorderArgs []string
	recordCtx    context.Context
	recordCancel context.CancelFunc
	recordCmd    *exec.Cmd
	recordOutput io.ReadCloser
	isMuted      bool

	playerApp  string
	playerArgs []string

	mutex sync.RWMutex
}

// NewAudioIO returns a new AudioIO struct based on the
// given configuration.
func NewAudioIO(config Config) *AudioIO {
	return &AudioIO{
		recorderApp:  config.Recording.Application,
		recorderArgs: config.Recording.ArgList(),
		playerApp:    config.Playback.Application,
		playerArgs:   config.Playback.ArgList(),
	}
}

// StartRecordingApp launches the user-specified audio input application.
func (audio *AudioIO) StartRecordingApp(ctx context.Context) error {
	// Ignore if we are already recording
	if audio.recordCancel != nil {
		return nil
	}

	// Create the command context so we can cancel it in the stop function.
	// This is how we can kill the external application.
	recordCtx, cancel := context.WithCancel(ctx)

	// Create the record command and get its stdout pipe
	cmd := exec.CommandContext(recordCtx,
		audio.recorderApp, audio.recorderArgs...)

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		cancel()
		return err
	}

	// Run the command
	if err = cmd.Start(); err != nil {
		cancel()
		return err
	}

	// Save the command
	audio.recordCmd = cmd
	audio.recordCtx = recordCtx
	audio.recordCancel = cancel
	audio.recordOutput = stdout

	return nil
}

// StopRecordingApp kills the user-sepcified audio input application.
func (audio *AudioIO) StopRecordingApp() error {
	// Ignore if it is alread stopped
	if audio.recordCancel == nil || audio.recordCmd == nil {
		return nil
	}

	// Cancel the context, which should kill the executable. Then wait
	// for it to finish.
	defer func() {
		audio.recordCtx = nil
		audio.recordCancel = nil
		audio.recordCmd = nil
		audio.recordOutput = nil
	}()

	audio.recordCancel()
	if err := audio.recordCmd.Wait(); err != nil {
		return err
	}

	return nil
}

// Mute the recording application. This prevents the audio from being
// pushed to the server while enabled, but still empties the stdout pipe
// of audio data.
func (audio *AudioIO) mute(enable bool) {
	audio.mutex.Lock()
	audio.isMuted = enable
	audio.mutex.Unlock()
}

// Read pulls the recorded audio data from the stdout pipe. This implements
// the io.Reader interfaces so that the struct can simply be passed to the
// PushAudio function (necessary for mute functionality)
func (audio *AudioIO) Read(p []byte) (n int, err error) {
	// Check if we are muted
	audio.mutex.RLock()
	ignore := audio.isMuted
	audio.mutex.RUnlock()

	if ignore {
		// Read from the stdout pipe, but instead of putting it into the
		// given buffer, we will put it in our own that is thrown away
		ignoreBuff := make([]byte, len(p))
		_, err := audio.recordOutput.Read(ignoreBuff)

		// Send back a byte of digital silence
		if len(p) > 0 {
			p[0] = 0
			return 1, nil
		}

		return 0, err
	}

	return audio.recordOutput.Read(p)
}

// Play the given audio data.
func (audio *AudioIO) Play(data []byte) error {

	// Mute the recording during playback.
	audio.mute(true)
	defer audio.mute(false)

	cmd := exec.Command(audio.playerApp, audio.playerArgs...)
	stdin, err := cmd.StdinPipe()
	if err != nil {
		return err
	}

	if err := cmd.Start(); err != nil {
		return err
	}

	if _, err := stdin.Write(data); err != nil {
		return err
	}

	if err := stdin.Close(); err != nil {
		return err
	}

	if err := cmd.Wait(); err != nil {
		return err
	}

	return nil
}
