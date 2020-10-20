//  ASRStream represents a stream of data sent from the client to
//  Diatheke for the purpose of speech recognition.
//
//  ASRStream.swift
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
import NIO

public class ASRStream {
    
    private let stream: ClientStreamingCall<Cobaltspeech_Diatheke_ASRInput, Cobaltspeech_Diatheke_ASRResult>
    
    public init(stream: ClientStreamingCall<Cobaltspeech_Diatheke_ASRInput, Cobaltspeech_Diatheke_ASRResult>) {
        self.stream = stream
    }
    
    // SendAudio to Diatheke for transcription. If the returned error
    // is io.EOF, the stream has already been closed by the server and
    // Result() should be called to get the final result.
    @discardableResult
    public func sendAudio(data: Data, completion: ((Error?) -> ())?) -> EventLoopFuture<Void> {
        var asrInput = Cobaltspeech_Diatheke_ASRInput()
        asrInput.data = .audio(data)
        let result = stream.sendMessage(asrInput)
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
    
    // SendToken passes the given session token to Diatheke to update
    // the speech recognition context. The session token must first be
    // sent on the ASR stream before any audio will be recognized. If
    // the stream  was created using client.NewSessionASRStream(), the
    //first token was already sent.
    //
    // If the returned error is io.EOF, the Result() function should be
    // called to get the final ASR result.
    @discardableResult
    public func sendToken(tokenData: Cobaltspeech_Diatheke_TokenData, completion: ((Error?) -> ())?) -> EventLoopFuture<Void> {
        var asrInput = Cobaltspeech_Diatheke_ASRInput()
        asrInput.data = .token(tokenData)
        let result = stream.sendMessage(asrInput)
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
    
    // Result returns the result of speech recognition. This function
    // may be called to end the audio stream early, which will force
    // a transcription based on the audio received until this point,
    // or in response to receiving an io.EOF error from PushAudio.
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
