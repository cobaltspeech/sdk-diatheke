//
//  Client.swift
//
//  Created by Eduard Miniakhmetov on 23.04.2020.
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
import NIOSSL

public typealias DiathekeFailureCallback = (_ error: Error) -> ()

public typealias Client = Cobaltspeech_Diatheke_DiathekeClient

public extension Cobaltspeech_Diatheke_DiathekeClient {
    
    convenience init(host: String,
                     port: Int,
                     useTLS: Bool = false) {
        let target = ConnectionTarget.hostAndPort(host, port)
        let eventLoopGroup = PlatformSupport.makeEventLoopGroup(loopCount: 1,
                                                                networkPreference: .best)
        
        let tls = useTLS ? ClientConnection.Configuration.TLS() : nil
            
        let configuration = ClientConnection.Configuration(target: target,
                                                           eventLoopGroup: eventLoopGroup,
                                                           errorDelegate: nil,
                                                           connectivityStateDelegate: nil,
                                                           tls: tls,
                                                           connectionBackoff: nil)
        let connection = ClientConnection(configuration: configuration)
        self.init(channel: connection)
    }
    
    convenience init(host: String,
                     port: Int,
                     tlsCertificateFileName: String,
                     tlsCertificateFormat: NIOSSLSerializationFormats) {
        let target = ConnectionTarget.hostAndPort(host, port)
        let eventLoopGroup = PlatformSupport.makeEventLoopGroup(loopCount: 1,
                                                                networkPreference: .best)
        
        var configuration = ClientConnection.Configuration(target: target, eventLoopGroup: eventLoopGroup)
        
        if let cert = try? NIOSSLCertificate(file: tlsCertificateFileName, format: tlsCertificateFormat) {
            let source = NIOSSLCertificateSource.certificate(cert)
            var tls = ClientConnection.Configuration.TLS()
            tls.certificateChain.append(source)
            configuration.tls = tls
        }
        
        let connection = ClientConnection(configuration: configuration)
        self.init(channel: connection)
    }
    
    // Version returns version information from the server.
    func version(success: @escaping (_ versionResponse: Cobaltspeech_Diatheke_VersionResponse) -> (),
                 failure: @escaping DiathekeFailureCallback) {
        version(Cobaltspeech_Diatheke_Empty()).response.whenComplete { (result) in
            switch result {
            case .success(let versionResponse):
                success(versionResponse)
            case .failure(let error):
                failure(error)
            }
        }
    }
    
    // ListModels returns a list of Diatheke models available to the server.
    func listModels(success: @escaping (_ models: [Cobaltspeech_Diatheke_ModelInfo]) -> (),
                    failure: @escaping DiathekeFailureCallback) {
        listModels(Cobaltspeech_Diatheke_Empty()).response.whenComplete { (result) in
            switch result {
            case .success(let response):
                success(response.models)
            case .failure(let error):
                failure(error)
            }
        }
    }
    
    fileprivate func createSession(_ modelID: String,
                                   _ wakeword: String?,
                                   _ success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                                   _ failure: @escaping DiathekeFailureCallback) {
       var request = Cobaltspeech_Diatheke_SessionStart()
       request.modelID = modelID
       
       if let wakeword = wakeword {
           request.wakeword = wakeword
       }

       createSession(request).response.whenComplete { (result) in
           switch result {
           case .success(let sessionOutput):
               success(sessionOutput)
           case .failure(let error):
               failure(error)
           }
       }
   }
    
    // CreateSession creates a new Diatheke session using the specified model.
    func createSession(modelID: String,
                       success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                       failure: @escaping DiathekeFailureCallback) {
        createSession(modelID, nil, success, failure)
    }
    
    // CreateSession creates a new Diatheke session with a custom wakeword
    // using the specified model.
    func createSession(modelID: String,
                       wakeword: String,
                       success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                       failure: @escaping DiathekeFailureCallback) {
        createSession(modelID, wakeword, success, failure)
    }

    // DeleteSession cleans up the given token. Behavior is undefined
    // if the given token is used again after calling this function.
    func deleteSession(tokenData: Cobaltspeech_Diatheke_TokenData,
                       success: (() -> ())? = nil,
                       failure: @escaping DiathekeFailureCallback) {
        deleteSession(tokenData).response.whenComplete { (result) in
            switch result {
            case .success(_):
                success?()
            case .failure(let error):
                failure(error)
            }
        }
    }
    
    // ProcessText sends the given text to Diatheke and returns an updated session token.
    @discardableResult
    func processText(token: Cobaltspeech_Diatheke_TokenData,
                     text: String,
                     success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                     failure: @escaping DiathekeFailureCallback) -> UnaryCall<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput> {
        var sessionInput = Cobaltspeech_Diatheke_SessionInput()
        sessionInput.token = token
        var textInput = Cobaltspeech_Diatheke_TextInput()
        textInput.text = text
        sessionInput.text = textInput
        
        let call = updateSession(sessionInput)
        call.response.whenComplete { (result) in
            switch result {
            case .success(let sessionOutput):
                success(sessionOutput)
            case .failure(let error):
                failure(error)
            }
        }
        
        return call
    }
    
    // ProcessASRResult sends the given ASR result to Diatheke and returns an
    // updated session token.
    @discardableResult
    func processASRResult(token: Cobaltspeech_Diatheke_TokenData,
                          asrResult: Cobaltspeech_Diatheke_ASRResult,
                          success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                          failure: @escaping DiathekeFailureCallback) -> UnaryCall<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput> {
        var sessionInput = Cobaltspeech_Diatheke_SessionInput()
        sessionInput.token = token
        sessionInput.asr = asrResult
        
        let call = updateSession(sessionInput)
        call.response.whenComplete { (result) in
            switch result {
            case .success(let sessionOutput):
                success(sessionOutput)
            case .failure(let error):
                failure(error)
            }
        }
        
        return call
    }
    
    // ProcessCommandResult sends the given command result to Diatheke and
    // returns an updated session token. This function should be called in
    // response to a command action Diatheke sent previously.
    @discardableResult
    func processCommandResult(token: Cobaltspeech_Diatheke_TokenData,
                              commandResult: Cobaltspeech_Diatheke_CommandResult,
                              success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                              failure: @escaping DiathekeFailureCallback) -> UnaryCall<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput> {
        var sessionInput = Cobaltspeech_Diatheke_SessionInput()
        sessionInput.token = token
        sessionInput.cmd = commandResult
        
        let call = updateSession(sessionInput)
        call.response.whenComplete { (result) in
            switch result {
            case .success(let sessionOutput):
                success(sessionOutput)
            case .failure(let error):
                failure(error)
            }
        }
        
        return call
    }
    
    // SetStory changes the current story for a Diatheke session. Returns
    // and updated session token.
    @discardableResult
    func setStory(token: Cobaltspeech_Diatheke_TokenData,
                  storyID: String,
                  params: [String : String],
                  success: @escaping (Cobaltspeech_Diatheke_SessionOutput) -> (),
                  failure: @escaping DiathekeFailureCallback) -> UnaryCall<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput> {
        var sessionInput = Cobaltspeech_Diatheke_SessionInput()
        sessionInput.token = token
       
        var setStory = Cobaltspeech_Diatheke_SetStory()
        setStory.storyID = storyID
        setStory.parameters = params
        sessionInput.story = setStory
        
        let call = updateSession(sessionInput)
        call.response.whenComplete { (result) in
            switch result {
            case .success(let sessionOutput):
                success(sessionOutput)
            case .failure(let error):
                failure(error)
            }
        }
        
        return call
    }
    
    // NewSessionASRStream creates a new stream to transcribe audio
    // for the given session Token.
    func newSessionASRStream(token: Cobaltspeech_Diatheke_TokenData,
                             asrResultHandler: @escaping (Result<Cobaltspeech_Diatheke_ASRResult, Error>) -> (),
                             completion: @escaping (Error?) -> ()) -> ASRStream {
        let stream = streamASR()
        stream.response.whenComplete { (result) in
            asrResultHandler(result)
        }
        let asrStream = ASRStream(stream: stream)
        asrStream.sendToken(tokenData: token) { (error) in
            completion(error)
        }
        return asrStream
    }
    
    // NewTTSStream creates a new stream to receive TTS audio from Diatheke
    // based on the given ReplyAction.
    func newTTSStream(replyAction: Cobaltspeech_Diatheke_ReplyAction,
                      dataChunkHandler: @escaping ((Cobaltspeech_Diatheke_TTSAudio) -> ()),
                      completion: @escaping (Error?) -> ()) -> TTSStream {
        let stream = streamTTS(replyAction, handler: dataChunkHandler)
        stream.status.whenComplete { (result) in
            switch result {
            case .success(let status):
                if status.isOk {
                    completion(nil)
                } else {
                    completion(status)
                }
            case .failure(let error):
                completion(error)
            }
        }
        let ttsStream = TTSStream(stream: stream)
        return ttsStream
    }
    
    // newTranscribe stream creates a new stream for audio transcription
    // unrelated to a session's state.
    func newTranscribeStream(action: Cobaltspeech_Diatheke_TranscribeAction,
                             transcribeResultHandler: @escaping ((Cobaltspeech_Diatheke_TranscribeResult) -> ()),
                             completion: @escaping (Error?) -> ()) -> TranscribeStream {
        let stream = self.transcribe(handler: transcribeResultHandler)
        
        stream.status.whenComplete { result in
            switch result {
            case .success(let status):
                if status.isOk {
                    completion(nil)
                } else {
                    completion(status)
                }
            case .failure(let error):
                completion(error)
            }
        }
    
        let transcribeStream = TranscribeStream(stream: stream)
        transcribeStream.sendAction(action, completion: completion)
        
        return transcribeStream
    }
    
}
