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

package main

// IsRecording returns true if audio is being recorded.
func (manager *sessionManager) IsRecording() bool {
	manager.mux.RLock()
	defer manager.mux.RUnlock()
	return manager.recording
}

// SetRecording sets the audio state for recording.
func (manager *sessionManager) SetRecording(on bool) {
	manager.mux.Lock()
	defer manager.mux.Unlock()
	manager.recording = on
}

// IsPlaying returns true if audio is being played.
func (manager *sessionManager) IsPlaying() bool {
	manager.mux.RLock()
	defer manager.mux.RUnlock()
	return manager.playing
}

// SetPlaying sets the audio state for playing.
func (manager *sessionManager) SetPlaying(on bool) {
	manager.mux.Lock()
	defer manager.mux.Unlock()
	manager.playing = on
}
