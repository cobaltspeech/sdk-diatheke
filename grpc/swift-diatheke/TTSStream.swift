//  TTSStream represents a stream of audio data from Diatheke to the
//  client containing synthesized speech.
//
//  TTSStream.swift
//  DiathekeSDKExample
//
//  Created by Eduard Miniakhmetov on 30.04.2020.
//  Copyright (2020) Cobalt Speech and Language Inc.

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

import Foundation
import GRPC

public class TTSStream {
    
    private let stream: ServerStreamingCall<Cobaltspeech_Diatheke_ReplyAction, Cobaltspeech_Diatheke_TTSAudio>
    
    public init(stream: ServerStreamingCall<Cobaltspeech_Diatheke_ReplyAction, Cobaltspeech_Diatheke_TTSAudio>) {
        self.stream = stream
    }
    
}
