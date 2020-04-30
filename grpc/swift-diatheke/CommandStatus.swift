//
//  CommandStatus.swift
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

public final class CommandStatus {
    
    public typealias ParamMap = [String : String]
    
    public enum StatusCode: Int {
        case success = 0
        case failure
    }
    
    public private(set) var commandID: String
    public private(set) var stateID: String
    public var statusCode: StatusCode
    public private(set) var params: ParamMap
    public var errorMesage: String
    
    
    public init(command: Cobaltspeech_Diatheke_CommandEvent) {
        commandID = command.commandID
        stateID = command.commandStateID
        statusCode = .failure
        errorMesage = ""
        params = command.parameters
    }
    
    public func setParam(key: String, value: Int) {
        params[key] = "\(value)"
    }
    
    public func setParam(key: String, value: Double) {
        params[key] = "\(value)"
    }
    
    public func setParam(key: String, value: String) {
        params[key] = value
    }
    
}
