//
// DO NOT EDIT.
//
// Generated by the protocol buffer compiler.
// Source: diatheke.proto
//

//
// Copyright 2018, gRPC Authors All rights reserved.
//
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
//
import GRPC
import NIO
import SwiftProtobuf


/// Usage: instantiate Cobaltspeech_Diatheke_DiathekeClient, then call methods of this protocol to make API calls.
public protocol Cobaltspeech_Diatheke_DiathekeClientProtocol: GRPCClient {
  func version(
    _ request: Cobaltspeech_Diatheke_Empty,
    callOptions: CallOptions?
  ) -> UnaryCall<Cobaltspeech_Diatheke_Empty, Cobaltspeech_Diatheke_VersionResponse>

  func listModels(
    _ request: Cobaltspeech_Diatheke_Empty,
    callOptions: CallOptions?
  ) -> UnaryCall<Cobaltspeech_Diatheke_Empty, Cobaltspeech_Diatheke_ListModelsResponse>

  func createSession(
    _ request: Cobaltspeech_Diatheke_SessionStart,
    callOptions: CallOptions?
  ) -> UnaryCall<Cobaltspeech_Diatheke_SessionStart, Cobaltspeech_Diatheke_SessionOutput>

  func deleteSession(
    _ request: Cobaltspeech_Diatheke_TokenData,
    callOptions: CallOptions?
  ) -> UnaryCall<Cobaltspeech_Diatheke_TokenData, Cobaltspeech_Diatheke_Empty>

  func updateSession(
    _ request: Cobaltspeech_Diatheke_SessionInput,
    callOptions: CallOptions?
  ) -> UnaryCall<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput>

  func streamASR(
    callOptions: CallOptions?
  ) -> ClientStreamingCall<Cobaltspeech_Diatheke_ASRInput, Cobaltspeech_Diatheke_ASRResult>

  func streamTTS(
    _ request: Cobaltspeech_Diatheke_ReplyAction,
    callOptions: CallOptions?,
    handler: @escaping (Cobaltspeech_Diatheke_TTSAudio) -> Void
  ) -> ServerStreamingCall<Cobaltspeech_Diatheke_ReplyAction, Cobaltspeech_Diatheke_TTSAudio>

}

extension Cobaltspeech_Diatheke_DiathekeClientProtocol {

  /// Returns version information from the server.
  ///
  /// - Parameters:
  ///   - request: Request to send to Version.
  ///   - callOptions: Call options.
  /// - Returns: A `UnaryCall` with futures for the metadata, status and response.
  public func version(
    _ request: Cobaltspeech_Diatheke_Empty,
    callOptions: CallOptions? = nil
  ) -> UnaryCall<Cobaltspeech_Diatheke_Empty, Cobaltspeech_Diatheke_VersionResponse> {
    return self.makeUnaryCall(
      path: "/cobaltspeech.diatheke.Diatheke/Version",
      request: request,
      callOptions: callOptions ?? self.defaultCallOptions
    )
  }

  /// ListModels returns information about the Diatheke models
  /// the server can access.
  ///
  /// - Parameters:
  ///   - request: Request to send to ListModels.
  ///   - callOptions: Call options.
  /// - Returns: A `UnaryCall` with futures for the metadata, status and response.
  public func listModels(
    _ request: Cobaltspeech_Diatheke_Empty,
    callOptions: CallOptions? = nil
  ) -> UnaryCall<Cobaltspeech_Diatheke_Empty, Cobaltspeech_Diatheke_ListModelsResponse> {
    return self.makeUnaryCall(
      path: "/cobaltspeech.diatheke.Diatheke/ListModels",
      request: request,
      callOptions: callOptions ?? self.defaultCallOptions
    )
  }

  /// Create a new Diatheke session. Also returns a list of
  /// actions to take next.
  ///
  /// - Parameters:
  ///   - request: Request to send to CreateSession.
  ///   - callOptions: Call options.
  /// - Returns: A `UnaryCall` with futures for the metadata, status and response.
  public func createSession(
    _ request: Cobaltspeech_Diatheke_SessionStart,
    callOptions: CallOptions? = nil
  ) -> UnaryCall<Cobaltspeech_Diatheke_SessionStart, Cobaltspeech_Diatheke_SessionOutput> {
    return self.makeUnaryCall(
      path: "/cobaltspeech.diatheke.Diatheke/CreateSession",
      request: request,
      callOptions: callOptions ?? self.defaultCallOptions
    )
  }

  /// Delete the session. Behavior is undefined if the given
  /// TokenData is used again after this function is called.
  ///
  /// - Parameters:
  ///   - request: Request to send to DeleteSession.
  ///   - callOptions: Call options.
  /// - Returns: A `UnaryCall` with futures for the metadata, status and response.
  public func deleteSession(
    _ request: Cobaltspeech_Diatheke_TokenData,
    callOptions: CallOptions? = nil
  ) -> UnaryCall<Cobaltspeech_Diatheke_TokenData, Cobaltspeech_Diatheke_Empty> {
    return self.makeUnaryCall(
      path: "/cobaltspeech.diatheke.Diatheke/DeleteSession",
      request: request,
      callOptions: callOptions ?? self.defaultCallOptions
    )
  }

  /// Process input for a session and get an updated session with
  /// a list of actions to take next. This is the only method
  /// that modifies the Diatheke session state.
  ///
  /// - Parameters:
  ///   - request: Request to send to UpdateSession.
  ///   - callOptions: Call options.
  /// - Returns: A `UnaryCall` with futures for the metadata, status and response.
  public func updateSession(
    _ request: Cobaltspeech_Diatheke_SessionInput,
    callOptions: CallOptions? = nil
  ) -> UnaryCall<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput> {
    return self.makeUnaryCall(
      path: "/cobaltspeech.diatheke.Diatheke/UpdateSession",
      request: request,
      callOptions: callOptions ?? self.defaultCallOptions
    )
  }

  /// Create an ASR stream. A result is returned when the
  /// stream is closed by the client (which forces the ASR to
  /// endpoint), or when a transcript becomes available on its
  /// own, in which case the stream is closed by the server.
  /// The ASR result may be used in the UpdateSession method.
  ///
  /// Callers should use the `send` method on the returned object to send messages
  /// to the server. The caller should send an `.end` after the final message has been sent.
  ///
  /// - Parameters:
  ///   - callOptions: Call options.
  /// - Returns: A `ClientStreamingCall` with futures for the metadata, status and response.
  public func streamASR(
    callOptions: CallOptions? = nil
  ) -> ClientStreamingCall<Cobaltspeech_Diatheke_ASRInput, Cobaltspeech_Diatheke_ASRResult> {
    return self.makeClientStreamingCall(
      path: "/cobaltspeech.diatheke.Diatheke/StreamASR",
      callOptions: callOptions ?? self.defaultCallOptions
    )
  }

  /// Create a TTS stream to receive audio for the given reply.
  /// The stream will close when TTS is finished. The client
  /// may also close the stream early to cancel the speech
  /// synthesis.
  ///
  /// - Parameters:
  ///   - request: Request to send to StreamTTS.
  ///   - callOptions: Call options.
  ///   - handler: A closure called when each response is received from the server.
  /// - Returns: A `ServerStreamingCall` with futures for the metadata and status.
  public func streamTTS(
    _ request: Cobaltspeech_Diatheke_ReplyAction,
    callOptions: CallOptions? = nil,
    handler: @escaping (Cobaltspeech_Diatheke_TTSAudio) -> Void
  ) -> ServerStreamingCall<Cobaltspeech_Diatheke_ReplyAction, Cobaltspeech_Diatheke_TTSAudio> {
    return self.makeServerStreamingCall(
      path: "/cobaltspeech.diatheke.Diatheke/StreamTTS",
      request: request,
      callOptions: callOptions ?? self.defaultCallOptions,
      handler: handler
    )
  }
}

public final class Cobaltspeech_Diatheke_DiathekeClient: Cobaltspeech_Diatheke_DiathekeClientProtocol {
  public let channel: GRPCChannel
  public var defaultCallOptions: CallOptions

  /// Creates a client for the cobaltspeech.diatheke.Diatheke service.
  ///
  /// - Parameters:
  ///   - channel: `GRPCChannel` to the service host.
  ///   - defaultCallOptions: Options to use for each service call if the user doesn't provide them.
  public init(channel: GRPCChannel, defaultCallOptions: CallOptions = CallOptions()) {
    self.channel = channel
    self.defaultCallOptions = defaultCallOptions
  }
}

