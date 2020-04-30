//
//  Session.swift
//
//  Created by Eduard Miniakhmetov on 24.04.2020.
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

public class Session {
    
    weak var client: Client!
    var id: String
    
    public init(id: String, client: Client) {
        self.id = id
        self.client = client
    }
    
    public func endSession(success: (() -> ())? = nil, failure: DiathekeFailureCallback?) {
        client.endSession(sessionID: id, success: success, failure: failure)
    }
    
    public func eventStream(handler: @escaping (Cobaltspeech_Diatheke_DiathekeEvent) -> ()) -> ServerStreamingCall<Cobaltspeech_Diatheke_SessionID, Cobaltspeech_Diatheke_DiathekeEvent> {
        let event = client.sessionEventStream(sessionID: id, handler: handler)
        return event
    }
    
    public func commandFinished(commandStatus: CommandStatus, failure: DiathekeFailureCallback? = nil) {
        client.commandFinished(sessionID: id, commandStatus: commandStatus, failure: failure)
    }
    
    public func pushText(_ text: String, failure: DiathekeFailureCallback?) {
        client.pushText(sessionID: id, text: text, failure: failure)
    }
    
    public func streamAudioInput(success: (() -> ())? = nil, failure: DiathekeFailureCallback?) -> AudioInputStream {
        return client.streamAudioInput(sessionID: id, success: success, failure: failure)
    }
    
    @discardableResult
    func streamAudioReplies(handler: @escaping (Cobaltspeech_Diatheke_AudioReply) -> ()) -> ServerStreamingCall<Cobaltspeech_Diatheke_SessionID, Cobaltspeech_Diatheke_AudioReply> {
        return client.streamAudioReplies(sessionID: id, handler: handler)
    }
    
}

