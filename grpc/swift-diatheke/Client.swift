//
//  Client.swift
//
//  Created by Eduard Miniakhmetov on 23.04.2020.
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
    
    func diathekeVersion(success: @escaping (_ version: String) -> (), failure: DiathekeFailureCallback?) {
        version(Cobaltspeech_Diatheke_Empty()).response.whenComplete { (result) in
            switch result {
            case .success(let response):
                success(response.server)
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
    func listModels(success: @escaping (_ models: [String]) -> (), failure: DiathekeFailureCallback?) {
        models(Cobaltspeech_Diatheke_Empty()).response.whenComplete { (result) in
            switch result {
            case .success(let response):
                success(response.models)
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
    func newSession(model: String,
                    success: @escaping (_ sessionID: String) -> (),
                    failure: DiathekeFailureCallback?) {
        var request = Cobaltspeech_Diatheke_NewSessionRequest()
        request.model = model
        
        newSession(request).response.whenComplete { (result) in
            switch result {
            case .success(let sessionID):
                success(sessionID.sessionID)
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
    func endSession(sessionID: String,
                    success: (() -> ())? = nil,
                    failure: DiathekeFailureCallback?) {
        var request = Cobaltspeech_Diatheke_SessionID()
        request.sessionID = sessionID
        
        endSession(request).response.whenComplete { (result) in
            switch result {
            case .success(_):
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
        
    }
    
    @discardableResult
    func sessionEventStream(sessionID: String, handler: @escaping (Cobaltspeech_Diatheke_DiathekeEvent) -> ()) -> ServerStreamingCall<Cobaltspeech_Diatheke_SessionID, Cobaltspeech_Diatheke_DiathekeEvent> {
        var request = Cobaltspeech_Diatheke_SessionID()
        request.sessionID = sessionID
        
        return sessionEventStream(request) { (diathekeEvent) in
            handler(diathekeEvent)
        }
    }
    
    func commandFinished(sessionID: String,
                         commandStatus: CommandStatus,
                         success: (() -> ())? = nil,
                         failure: DiathekeFailureCallback? = nil) {
        var request = Cobaltspeech_Diatheke_CommandStatus()
        request.sessionID = sessionID
        request.commandID = commandStatus.commandID
        
        switch commandStatus.statusCode {
        case .success:
            request.returnStatus = .success
        case .failure:
            request.returnStatus = .failure
        }

        request.outputParameters = commandStatus.params
        request.errorMessageText = commandStatus.errorMesage
        request.commandStateID = commandStatus.stateID
        
        commandFinished(request).response.whenComplete { (result) in
            switch result {
            case .success(_):
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
    func streamAudioInput(sessionID: String, success: (() -> ())? = nil, failure: DiathekeFailureCallback?) -> AudioInputStream {
        let stream = streamAudioInput()
        var request = Cobaltspeech_Diatheke_AudioInput()
        request.sessionID = sessionID
        
        stream.sendMessage(request).whenComplete { (result) in
            switch result {
            case .success():
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
        
        let audioInputStream = AudioInputStream(stream: stream)
    
        return audioInputStream
    }
    
    @discardableResult
    func streamAudioReplies(sessionID: String, handler: @escaping (Cobaltspeech_Diatheke_AudioReply) -> ()) -> ServerStreamingCall<Cobaltspeech_Diatheke_SessionID, Cobaltspeech_Diatheke_AudioReply> {
        var request = Cobaltspeech_Diatheke_SessionID()
        request.sessionID = sessionID
        
        let reply = streamAudioReplies(request) { (reply) in
            handler(reply)
        }
        
        return reply
    }
    
    func pushText(sessionID: String,
                  text: String,
                  success: (() -> ())? = nil,
                  failure: DiathekeFailureCallback?) {
        var request = Cobaltspeech_Diatheke_PushTextRequest()
        request.sessionID = sessionID
        request.text = text
        
        pushText(request).response.whenComplete { (result) in
            switch result {
            case .success(_):
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
    }
    
    func streamASR(model: String,
                   success: (() -> ())? = nil,
                   failure: DiathekeFailureCallback? = nil,
                   handler: @escaping (Cobaltspeech_Diatheke_ASRResponse) -> ()) -> ASRStream {
        var asrRequest = Cobaltspeech_Diatheke_ASRRequest()
        asrRequest.model = model
        
        let stream = streamASR { (response) in
            handler(response)
        }
        
        stream.sendMessage(asrRequest).whenComplete { (result) in
            switch result {
            case .success():
                success?()
            case .failure(let error):
                failure?(error)
            }
        }
        
        return ASRStream(stream: stream)
    }
    
    @discardableResult
    func streamTTS(model: String,
                   text: String,
                   handler: @escaping (Cobaltspeech_Diatheke_TTSResponse) -> ()) -> ServerStreamingCall<Cobaltspeech_Diatheke_TTSRequest, Cobaltspeech_Diatheke_TTSResponse> {
        var request = Cobaltspeech_Diatheke_TTSRequest()
        request.model = model
        request.text = text
        
        let response = streamTTS(request) { (response) in
            handler(response)
        }
    
        return response
    }
    
}
