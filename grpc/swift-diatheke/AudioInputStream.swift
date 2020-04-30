//
//  AudioInputStream.swift
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

public typealias AudioReplyStream = ServerStreamingCall<Cobaltspeech_Diatheke_SessionID, Cobaltspeech_Diatheke_AudioReply>

public class AudioInputStream {
    
    private let stream: ClientStreamingCall<Cobaltspeech_Diatheke_AudioInput, Cobaltspeech_Diatheke_Empty>
    
    public init(stream: ClientStreamingCall<Cobaltspeech_Diatheke_AudioInput, Cobaltspeech_Diatheke_Empty>) {
        self.stream = stream
    }
    
    public func pushAudio(data: Data, success: (() -> ())? = nil, failure: DiathekeFailureCallback? = nil) {
        var request = Cobaltspeech_Diatheke_AudioInput()
        request.data = data
        
        stream.sendMessage(request).whenComplete { (result) in
            switch result {
            case .success():
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
    public func finish(success: (() -> ())? = nil, failure: DiathekeFailureCallback? = nil) {
        stream.sendEnd().whenComplete { (result) in
            switch result {
            case .success():
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
}
