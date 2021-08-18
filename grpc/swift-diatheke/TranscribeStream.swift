//  TranscribeStream represents a stream of data sent from the client
//  to Diatheke for the purpose of transcribing speech.
//
//  TranscribeStream.swift
//
//  Created by Eduard Miniakhmetov on 18.08.2021.
//  Copyright (2021) Cobalt Speech and Language Inc.

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
import NIO

public class TranscribeStream {
    
    private let stream: BidirectionalStreamingCall<Cobaltspeech_Diatheke_TranscribeInput, Cobaltspeech_Diatheke_TranscribeResult>
    
    public init(stream: BidirectionalStreamingCall<Cobaltspeech_Diatheke_TranscribeInput, Cobaltspeech_Diatheke_TranscribeResult>) {
        self.stream = stream
    }
    
    // SendAudio to Diatheke for transcription.
    @discardableResult
    public func sendAudio(_ data: Data, completion: ((Error?) -> ())?) -> EventLoopFuture<Void> {
        var transcribeInput = Cobaltspeech_Diatheke_TranscribeInput()
        transcribeInput.audio = data
        let result = stream.sendMessage(transcribeInput)
        
        result.whenComplete { response in
            switch response {
            case .success():
                completion?(nil)
            case .failure(let error):
                completion?(error)
            }
        }
        
        return result
    }
    
    // SendAction passes the given transcribe action to Diatheke to use
    // for speech recognition. The action must first be sent before
    // any audio will be transcribed. If the stream was created using
    // client.newTranscribeStream(), the action was already sent.
    @discardableResult
    public func sendAction(_ action: Cobaltspeech_Diatheke_TranscribeAction, completion: ((Error?) -> ())?) -> EventLoopFuture<Void> {
        var transcribeInput = Cobaltspeech_Diatheke_TranscribeInput()
        transcribeInput.action = action
        let result = stream.sendMessage(transcribeInput)
        
        result.whenComplete { response in
            switch response {
            case .success():
                completion?(nil)
            case .failure(let error):
                completion?(error)
            }
        }
        
        return result
    }
    
    // result waits for the next transcribe result from
    // Diatheke. When the returned error is io.EOF, the stream has
    // been closed and no more results will be sent from Diatheke.
    @discardableResult
    public func result(completion: ((Error?) -> ())?) -> EventLoopFuture<Void> {
        let result = stream.sendEnd()
        result.whenComplete { (result) in
            switch result {
            case .success():
                completion?(nil)
            case .failure(let error):
                completion?(error)
            }
        }
        return result
    }
    
}
