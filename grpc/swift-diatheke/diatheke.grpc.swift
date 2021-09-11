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


/// Service that implements the Cobalt Diatheke Dialog Management API.
///
/// Usage: instantiate `Cobaltspeech_Diatheke_DiathekeClient`, then call methods of this protocol to make API calls.
public protocol Cobaltspeech_Diatheke_DiathekeClientProtocol: GRPCClient {
  var serviceName: String { get }
  var interceptors: Cobaltspeech_Diatheke_DiathekeClientInterceptorFactoryProtocol? { get }

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

  func transcribe(
    callOptions: CallOptions?,
    handler: @escaping (Cobaltspeech_Diatheke_TranscribeResult) -> Void
  ) -> BidirectionalStreamingCall<Cobaltspeech_Diatheke_TranscribeInput, Cobaltspeech_Diatheke_TranscribeResult>
}

extension Cobaltspeech_Diatheke_DiathekeClientProtocol {
  public var serviceName: String {
    return "cobaltspeech.diatheke.Diatheke"
  }

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
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeVersionInterceptors() ?? []
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
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeListModelsInterceptors() ?? []
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
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeCreateSessionInterceptors() ?? []
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
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeDeleteSessionInterceptors() ?? []
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
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeUpdateSessionInterceptors() ?? []
    )
  }

  /// Create an ASR stream. A result is returned when the
  /// stream is closed by the client (which forces the ASR to
  /// endpoint), or when a transcript becomes available on its
  /// own, in which case the stream is closed by the server.
  /// The ASR result may be used in the UpdateSession method.
  /// <br/><br/>
  /// If the session has a wakeword enabled, and the client
  /// application is using Diatheke and Cubic to handle the
  /// wakeword processing, this method will not return a
  /// result until the wakeword condition has been satisfied.
  /// Utterances without the required wakeword will be
  /// discarded and no transcription will be returned.
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
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeStreamASRInterceptors() ?? []
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
      interceptors: self.interceptors?.makeStreamTTSInterceptors() ?? [],
      handler: handler
    )
  }

  /// Create an ASR stream for transcription. Unlike StreamASR,
  /// Transcribe does not listen for a wakeword. This method
  /// returns a bi-directional stream, and its intended use is
  /// for situations where a user may say anything at all, whether
  /// it is short or long, and the application wants to save the
  /// transcript (e.g., take a note, send a message).
  /// <br/><br/>
  /// The first message sent to the server must be the TranscribeAction,
  /// with remaining messages sending audio data.
  /// Messages received from the server will include the current
  /// best partial transcription until the full transcription is
  /// ready. The stream ends when either the client application
  /// closes it, a predefined duration of silence (non-speech)
  /// occurs, or the end-transcription intent is recognized.
  ///
  /// Callers should use the `send` method on the returned object to send messages
  /// to the server. The caller should send an `.end` after the final message has been sent.
  ///
  /// - Parameters:
  ///   - callOptions: Call options.
  ///   - handler: A closure called when each response is received from the server.
  /// - Returns: A `ClientStreamingCall` with futures for the metadata and status.
  public func transcribe(
    callOptions: CallOptions? = nil,
    handler: @escaping (Cobaltspeech_Diatheke_TranscribeResult) -> Void
  ) -> BidirectionalStreamingCall<Cobaltspeech_Diatheke_TranscribeInput, Cobaltspeech_Diatheke_TranscribeResult> {
    return self.makeBidirectionalStreamingCall(
      path: "/cobaltspeech.diatheke.Diatheke/Transcribe",
      callOptions: callOptions ?? self.defaultCallOptions,
      interceptors: self.interceptors?.makeTranscribeInterceptors() ?? [],
      handler: handler
    )
  }
}

public protocol Cobaltspeech_Diatheke_DiathekeClientInterceptorFactoryProtocol {

  /// - Returns: Interceptors to use when invoking 'version'.
  func makeVersionInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_Empty, Cobaltspeech_Diatheke_VersionResponse>]

  /// - Returns: Interceptors to use when invoking 'listModels'.
  func makeListModelsInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_Empty, Cobaltspeech_Diatheke_ListModelsResponse>]

  /// - Returns: Interceptors to use when invoking 'createSession'.
  func makeCreateSessionInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_SessionStart, Cobaltspeech_Diatheke_SessionOutput>]

  /// - Returns: Interceptors to use when invoking 'deleteSession'.
  func makeDeleteSessionInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_TokenData, Cobaltspeech_Diatheke_Empty>]

  /// - Returns: Interceptors to use when invoking 'updateSession'.
  func makeUpdateSessionInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_SessionInput, Cobaltspeech_Diatheke_SessionOutput>]

  /// - Returns: Interceptors to use when invoking 'streamASR'.
  func makeStreamASRInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_ASRInput, Cobaltspeech_Diatheke_ASRResult>]

  /// - Returns: Interceptors to use when invoking 'streamTTS'.
  func makeStreamTTSInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_ReplyAction, Cobaltspeech_Diatheke_TTSAudio>]

  /// - Returns: Interceptors to use when invoking 'transcribe'.
  func makeTranscribeInterceptors() -> [ClientInterceptor<Cobaltspeech_Diatheke_TranscribeInput, Cobaltspeech_Diatheke_TranscribeResult>]
}

public final class Cobaltspeech_Diatheke_DiathekeClient: Cobaltspeech_Diatheke_DiathekeClientProtocol {
  public let channel: GRPCChannel
  public var defaultCallOptions: CallOptions
  public var interceptors: Cobaltspeech_Diatheke_DiathekeClientInterceptorFactoryProtocol?

  /// Creates a client for the cobaltspeech.diatheke.Diatheke service.
  ///
  /// - Parameters:
  ///   - channel: `GRPCChannel` to the service host.
  ///   - defaultCallOptions: Options to use for each service call if the user doesn't provide them.
  ///   - interceptors: A factory providing interceptors for each RPC.
  public init(
    channel: GRPCChannel,
    defaultCallOptions: CallOptions = CallOptions(),
    interceptors: Cobaltspeech_Diatheke_DiathekeClientInterceptorFactoryProtocol? = nil
  ) {
    self.channel = channel
    self.defaultCallOptions = defaultCallOptions
    self.interceptors = interceptors
  }
}

