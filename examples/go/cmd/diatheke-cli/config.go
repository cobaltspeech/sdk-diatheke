package main

import (
	"fmt"
	"os"
	"os/exec"
	"strings"

	"github.com/BurntSushi/toml"
)

// ServerConfig contains the Diatheke server settings
type ServerConfig struct {
	Address  string
	Insecure bool
}

// AudioConfig contains the settings for audio I/O
type AudioConfig struct {
	Application string
	Args        string
}

// ArgList returns the arguments as a list of strings.
func (cfg AudioConfig) ArgList() []string {
	return strings.Fields(cfg.Args)
}

// Config contains the application configuration
type Config struct {
	Server    ServerConfig
	UseAudio  bool
	Recording AudioConfig
	Playback  AudioConfig
}

// ReadConfigFile attempts to load the given config file
func ReadConfigFile(filename string) (Config, error) {
	var config Config

	_, err := toml.DecodeFile(filename, &config)
	if err != nil {
		return config, err
	}

	if config.Server.Address == "" {
		return config, fmt.Errorf("missing server address")
	}

	// If the client is using audio, make sure the recording
	// and playback applications are set and can be found.
	if config.UseAudio {
		if err := checkAudioConfig(config.Recording); err != nil {
			return config, fmt.Errorf("recording config error - %v", err)
		}

		if err := checkAudioConfig(config.Playback); err != nil {
			return config, fmt.Errorf("playback config error - %v", err)
		}
	}

	return config, nil
}

func checkAudioConfig(config AudioConfig) error {
	// Make sure the application was specified
	if config.Application == "" {
		return fmt.Errorf("application not specified")
	}

	// Verify that the file (executable) exists
	info, err := os.Stat(config.Application)
	if err != nil {
		// This is a path error, which means we couldn't find the file.
		// Check the system path to see if we can find it there.
		_, err = exec.LookPath(config.Application)
		if err != nil {
			return fmt.Errorf("could not find application %s", config.Application)
		}
	} else if info.IsDir() {
		return fmt.Errorf("application is a directory, not an executable")
	}

	return nil
}
