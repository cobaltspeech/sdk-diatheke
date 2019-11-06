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

package config

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
	ModelID  string
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
	Verbose   bool
	Server    ServerConfig
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

	// If the recording or playback fields are set, check them.
	if config.Recording.Application != "" {
		if err := checkAudioConfig(config.Recording.Application); err != nil {
			return config, fmt.Errorf("recording config error - %v", err)
		}
	}

	if config.Playback.Application != "" {
		if err := checkAudioConfig(config.Playback.Application); err != nil {
			return config, fmt.Errorf("playback config error - %v", err)
		}
	}

	return config, nil
}

func checkAudioConfig(app string) error {
	// Verify that the file (executable) exists
	info, err := os.Stat(app)
	if err != nil {
		// This is a path error, which means we couldn't find the file.
		// Check the system path to see if we can find it there.
		_, err = exec.LookPath(app)
		if err != nil {
			return fmt.Errorf("could not find application %s", app)
		}
	} else if info.IsDir() {
		return fmt.Errorf("application is a directory, not an executable")
	}

	return nil
}
