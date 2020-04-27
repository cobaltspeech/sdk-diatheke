package cobaltspeech.diatheke;

import static io.grpc.MethodDescriptor.generateFullMethodName;
import static io.grpc.stub.ClientCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ClientCalls.asyncClientStreamingCall;
import static io.grpc.stub.ClientCalls.asyncServerStreamingCall;
import static io.grpc.stub.ClientCalls.asyncUnaryCall;
import static io.grpc.stub.ClientCalls.blockingServerStreamingCall;
import static io.grpc.stub.ClientCalls.blockingUnaryCall;
import static io.grpc.stub.ClientCalls.futureUnaryCall;
import static io.grpc.stub.ServerCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ServerCalls.asyncClientStreamingCall;
import static io.grpc.stub.ServerCalls.asyncServerStreamingCall;
import static io.grpc.stub.ServerCalls.asyncUnaryCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedStreamingCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedUnaryCall;

/**
 * <pre>
 * Service that implements the Cobalt Diatheke Dialog Management API.
 * </pre>
 */
@javax.annotation.Generated(
    value = "by gRPC proto compiler (version 1.24.0)",
    comments = "Source: diatheke.proto")
public final class DiathekeGrpc {

  private DiathekeGrpc() {}

  public static final String SERVICE_NAME = "cobaltspeech.diatheke.Diatheke";

  // Static method descriptors that strictly reflect the proto.
  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.Empty,
      cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse> getVersionMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "Version",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.Empty.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.Empty,
      cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse> getVersionMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.Empty, cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse> getVersionMethod;
    if ((getVersionMethod = DiathekeGrpc.getVersionMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getVersionMethod = DiathekeGrpc.getVersionMethod) == null) {
          DiathekeGrpc.getVersionMethod = getVersionMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.Empty, cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "Version"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse.getDefaultInstance()))
              .build();
        }
      }
    }
    return getVersionMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.Empty,
      cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse> getModelsMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "Models",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.Empty.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.Empty,
      cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse> getModelsMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.Empty, cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse> getModelsMethod;
    if ((getModelsMethod = DiathekeGrpc.getModelsMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getModelsMethod = DiathekeGrpc.getModelsMethod) == null) {
          DiathekeGrpc.getModelsMethod = getModelsMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.Empty, cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "Models"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse.getDefaultInstance()))
              .build();
        }
      }
    }
    return getModelsMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.SessionID> getNewSessionMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "NewSession",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.SessionID.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.SessionID> getNewSessionMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest, cobaltspeech.diatheke.DiathekeOuterClass.SessionID> getNewSessionMethod;
    if ((getNewSessionMethod = DiathekeGrpc.getNewSessionMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getNewSessionMethod = DiathekeGrpc.getNewSessionMethod) == null) {
          DiathekeGrpc.getNewSessionMethod = getNewSessionMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest, cobaltspeech.diatheke.DiathekeOuterClass.SessionID>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "NewSession"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.SessionID.getDefaultInstance()))
              .build();
        }
      }
    }
    return getNewSessionMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getEndSessionMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "EndSession",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.SessionID.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.Empty.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getEndSessionMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID, cobaltspeech.diatheke.DiathekeOuterClass.Empty> getEndSessionMethod;
    if ((getEndSessionMethod = DiathekeGrpc.getEndSessionMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getEndSessionMethod = DiathekeGrpc.getEndSessionMethod) == null) {
          DiathekeGrpc.getEndSessionMethod = getEndSessionMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.SessionID, cobaltspeech.diatheke.DiathekeOuterClass.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "EndSession"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.SessionID.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.Empty.getDefaultInstance()))
              .build();
        }
      }
    }
    return getEndSessionMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
      cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent> getSessionEventStreamMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SessionEventStream",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.SessionID.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent.class,
      methodType = io.grpc.MethodDescriptor.MethodType.SERVER_STREAMING)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
      cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent> getSessionEventStreamMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID, cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent> getSessionEventStreamMethod;
    if ((getSessionEventStreamMethod = DiathekeGrpc.getSessionEventStreamMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getSessionEventStreamMethod = DiathekeGrpc.getSessionEventStreamMethod) == null) {
          DiathekeGrpc.getSessionEventStreamMethod = getSessionEventStreamMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.SessionID, cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.SERVER_STREAMING)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "SessionEventStream"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.SessionID.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent.getDefaultInstance()))
              .build();
        }
      }
    }
    return getSessionEventStreamMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getCommandFinishedMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "CommandFinished",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.Empty.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getCommandFinishedMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus, cobaltspeech.diatheke.DiathekeOuterClass.Empty> getCommandFinishedMethod;
    if ((getCommandFinishedMethod = DiathekeGrpc.getCommandFinishedMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getCommandFinishedMethod = DiathekeGrpc.getCommandFinishedMethod) == null) {
          DiathekeGrpc.getCommandFinishedMethod = getCommandFinishedMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus, cobaltspeech.diatheke.DiathekeOuterClass.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "CommandFinished"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.Empty.getDefaultInstance()))
              .build();
        }
      }
    }
    return getCommandFinishedMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.AudioInput,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getStreamAudioInputMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "StreamAudioInput",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.AudioInput.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.Empty.class,
      methodType = io.grpc.MethodDescriptor.MethodType.CLIENT_STREAMING)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.AudioInput,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getStreamAudioInputMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.AudioInput, cobaltspeech.diatheke.DiathekeOuterClass.Empty> getStreamAudioInputMethod;
    if ((getStreamAudioInputMethod = DiathekeGrpc.getStreamAudioInputMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getStreamAudioInputMethod = DiathekeGrpc.getStreamAudioInputMethod) == null) {
          DiathekeGrpc.getStreamAudioInputMethod = getStreamAudioInputMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.AudioInput, cobaltspeech.diatheke.DiathekeOuterClass.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.CLIENT_STREAMING)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "StreamAudioInput"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.AudioInput.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.Empty.getDefaultInstance()))
              .build();
        }
      }
    }
    return getStreamAudioInputMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
      cobaltspeech.diatheke.DiathekeOuterClass.AudioReply> getStreamAudioRepliesMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "StreamAudioReplies",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.SessionID.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.AudioReply.class,
      methodType = io.grpc.MethodDescriptor.MethodType.SERVER_STREAMING)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
      cobaltspeech.diatheke.DiathekeOuterClass.AudioReply> getStreamAudioRepliesMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.SessionID, cobaltspeech.diatheke.DiathekeOuterClass.AudioReply> getStreamAudioRepliesMethod;
    if ((getStreamAudioRepliesMethod = DiathekeGrpc.getStreamAudioRepliesMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getStreamAudioRepliesMethod = DiathekeGrpc.getStreamAudioRepliesMethod) == null) {
          DiathekeGrpc.getStreamAudioRepliesMethod = getStreamAudioRepliesMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.SessionID, cobaltspeech.diatheke.DiathekeOuterClass.AudioReply>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.SERVER_STREAMING)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "StreamAudioReplies"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.SessionID.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.AudioReply.getDefaultInstance()))
              .build();
        }
      }
    }
    return getStreamAudioRepliesMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getPushTextMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PushText",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.Empty.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.Empty> getPushTextMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest, cobaltspeech.diatheke.DiathekeOuterClass.Empty> getPushTextMethod;
    if ((getPushTextMethod = DiathekeGrpc.getPushTextMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getPushTextMethod = DiathekeGrpc.getPushTextMethod) == null) {
          DiathekeGrpc.getPushTextMethod = getPushTextMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest, cobaltspeech.diatheke.DiathekeOuterClass.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "PushText"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.Empty.getDefaultInstance()))
              .build();
        }
      }
    }
    return getPushTextMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse> getStreamASRMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "StreamASR",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.BIDI_STREAMING)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse> getStreamASRMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest, cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse> getStreamASRMethod;
    if ((getStreamASRMethod = DiathekeGrpc.getStreamASRMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getStreamASRMethod = DiathekeGrpc.getStreamASRMethod) == null) {
          DiathekeGrpc.getStreamASRMethod = getStreamASRMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest, cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.BIDI_STREAMING)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "StreamASR"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse.getDefaultInstance()))
              .build();
        }
      }
    }
    return getStreamASRMethod;
  }

  private static volatile io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse> getStreamTTSMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "StreamTTS",
      requestType = cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest.class,
      responseType = cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.SERVER_STREAMING)
  public static io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest,
      cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse> getStreamTTSMethod() {
    io.grpc.MethodDescriptor<cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest, cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse> getStreamTTSMethod;
    if ((getStreamTTSMethod = DiathekeGrpc.getStreamTTSMethod) == null) {
      synchronized (DiathekeGrpc.class) {
        if ((getStreamTTSMethod = DiathekeGrpc.getStreamTTSMethod) == null) {
          DiathekeGrpc.getStreamTTSMethod = getStreamTTSMethod =
              io.grpc.MethodDescriptor.<cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest, cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.SERVER_STREAMING)
              .setFullMethodName(generateFullMethodName(SERVICE_NAME, "StreamTTS"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.lite.ProtoLiteUtils.marshaller(
                  cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse.getDefaultInstance()))
              .build();
        }
      }
    }
    return getStreamTTSMethod;
  }

  /**
   * Creates a new async stub that supports all call types for the service
   */
  public static DiathekeStub newStub(io.grpc.Channel channel) {
    return new DiathekeStub(channel);
  }

  /**
   * Creates a new blocking-style stub that supports unary and streaming output calls on the service
   */
  public static DiathekeBlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new DiathekeBlockingStub(channel);
  }

  /**
   * Creates a new ListenableFuture-style stub that supports unary calls on the service
   */
  public static DiathekeFutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new DiathekeFutureStub(channel);
  }

  /**
   * <pre>
   * Service that implements the Cobalt Diatheke Dialog Management API.
   * </pre>
   */
  public static abstract class DiathekeImplBase implements io.grpc.BindableService {

    /**
     * <pre>
     * Queries the Version of the Server.
     * </pre>
     */
    public void version(cobaltspeech.diatheke.DiathekeOuterClass.Empty request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getVersionMethod(), responseObserver);
    }

    /**
     * <pre>
     * Models will return a list of available versions.  Model values from
     * this list may be used in NewSession calls.
     * </pre>
     */
    public void models(cobaltspeech.diatheke.DiathekeOuterClass.Empty request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getModelsMethod(), responseObserver);
    }

    /**
     * <pre>
     * Requests a new session with the given config and returns the session
     * ID, which is required for other rpc methods.
     * </pre>
     */
    public void newSession(cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.SessionID> responseObserver) {
      asyncUnimplementedUnaryCall(getNewSessionMethod(), responseObserver);
    }

    /**
     * <pre>
     * Terminates an existing session and closes any open event streams.
     * It is an error if the SessionEndRequest has an invalid SessionID.
     * </pre>
     */
    public void endSession(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getEndSessionMethod(), responseObserver);
    }

    /**
     * <pre>
     * Requests a new event stream for the given session.
     * </pre>
     */
    public void sessionEventStream(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent> responseObserver) {
      asyncUnimplementedUnaryCall(getSessionEventStreamMethod(), responseObserver);
    }

    /**
     * <pre>
     * Notify Diatheke when a command has completed so that it may update
     * the dialog state. The initial command request will come as part of
     * a DiathekeEvent. While not strictly required (depeding on the model
     * and command), it is best practice to always call this method when a
     * command is complete. Cases where it is required include when the 
     * command has output parameters, or when the command is followed by 
     * another action in the Diatheke model.
     * </pre>
     */
    public void commandFinished(cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getCommandFinishedMethod(), responseObserver);
    }

    /**
     * <pre>
     * Begin an audio input stream for a session. The first message to
     * the server should specify the sessionID, with binary audio data pushed
     * for every subsequent message. As the audio is recognized, Diatheke
     * will respond with appropriate events on the session's event stream.
     * &lt;p&gt;
     * While it is allowed to call this multiple times during a single session,
     * clients should never have multiple audio input streams running concurrently
     * for the same session (the audio may mix and result in unpredictable
     * behavior). Previous audio streams should always be closed before starting
     * a new one.
     * </pre>
     */
    public io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.AudioInput> streamAudioInput(
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      return asyncUnimplementedStreamingCall(getStreamAudioInputMethod(), responseObserver);
    }

    /**
     * <pre>
     * Create an audio reply stream for a session. The returned stream 
     * will receive replies ("say" entries in the Diatheke model) from the
     * server as they occur in the conversation. For each "say" entry, the 
     * stream will first receive the text to synthesize (defined by the model),
     * followed by one or more messages containing the synthesized audio bytes. 
     * The "say" entry will end with a message indicating that TTS for that 
     * entry is complete.
     * NOTE: The text in the first message of an audio reply is the same that
     *       will be received in the session's event stream.
     * </pre>
     */
    public void streamAudioReplies(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.AudioReply> responseObserver) {
      asyncUnimplementedUnaryCall(getStreamAudioRepliesMethod(), responseObserver);
    }

    /**
     * <pre>
     * Push text to Diatheke as part of the conversation for a session.
     * Diatheke will respond with an appropriate event on the session's
     * event stream based on whether the given text was recognized as a
     * valid intent or not.
     * </pre>
     */
    public void pushText(cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getPushTextMethod(), responseObserver);
    }

    /**
     * <pre>
     * Manually run streaming ASR unrelated to any session by pushing
     * audio data to the server on the audio stream. As transcriptions
     * become available, the server will return them on the ASRResponse
     * stream. The transcriptions may then be used for, e.g., the PushText
     * method. This function is provided as a convenience.
     * </pre>
     */
    public io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest> streamASR(
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse> responseObserver) {
      return asyncUnimplementedStreamingCall(getStreamASRMethod(), responseObserver);
    }

    /**
     * <pre>
     * Manually run streaming TTS. The Audio stream will receive
     * binary audio data as it is synthesized and will close automatically
     * when synthesis is complete. This function is provided as a
     * convenience.
     * </pre>
     */
    public void streamTTS(cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getStreamTTSMethod(), responseObserver);
    }

    @java.lang.Override public final io.grpc.ServerServiceDefinition bindService() {
      return io.grpc.ServerServiceDefinition.builder(getServiceDescriptor())
          .addMethod(
            getVersionMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.Empty,
                cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse>(
                  this, METHODID_VERSION)))
          .addMethod(
            getModelsMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.Empty,
                cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse>(
                  this, METHODID_MODELS)))
          .addMethod(
            getNewSessionMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest,
                cobaltspeech.diatheke.DiathekeOuterClass.SessionID>(
                  this, METHODID_NEW_SESSION)))
          .addMethod(
            getEndSessionMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
                cobaltspeech.diatheke.DiathekeOuterClass.Empty>(
                  this, METHODID_END_SESSION)))
          .addMethod(
            getSessionEventStreamMethod(),
            asyncServerStreamingCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
                cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent>(
                  this, METHODID_SESSION_EVENT_STREAM)))
          .addMethod(
            getCommandFinishedMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus,
                cobaltspeech.diatheke.DiathekeOuterClass.Empty>(
                  this, METHODID_COMMAND_FINISHED)))
          .addMethod(
            getStreamAudioInputMethod(),
            asyncClientStreamingCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.AudioInput,
                cobaltspeech.diatheke.DiathekeOuterClass.Empty>(
                  this, METHODID_STREAM_AUDIO_INPUT)))
          .addMethod(
            getStreamAudioRepliesMethod(),
            asyncServerStreamingCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.SessionID,
                cobaltspeech.diatheke.DiathekeOuterClass.AudioReply>(
                  this, METHODID_STREAM_AUDIO_REPLIES)))
          .addMethod(
            getPushTextMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest,
                cobaltspeech.diatheke.DiathekeOuterClass.Empty>(
                  this, METHODID_PUSH_TEXT)))
          .addMethod(
            getStreamASRMethod(),
            asyncBidiStreamingCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest,
                cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse>(
                  this, METHODID_STREAM_ASR)))
          .addMethod(
            getStreamTTSMethod(),
            asyncServerStreamingCall(
              new MethodHandlers<
                cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest,
                cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse>(
                  this, METHODID_STREAM_TTS)))
          .build();
    }
  }

  /**
   * <pre>
   * Service that implements the Cobalt Diatheke Dialog Management API.
   * </pre>
   */
  public static final class DiathekeStub extends io.grpc.stub.AbstractStub<DiathekeStub> {
    private DiathekeStub(io.grpc.Channel channel) {
      super(channel);
    }

    private DiathekeStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected DiathekeStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new DiathekeStub(channel, callOptions);
    }

    /**
     * <pre>
     * Queries the Version of the Server.
     * </pre>
     */
    public void version(cobaltspeech.diatheke.DiathekeOuterClass.Empty request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getVersionMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Models will return a list of available versions.  Model values from
     * this list may be used in NewSession calls.
     * </pre>
     */
    public void models(cobaltspeech.diatheke.DiathekeOuterClass.Empty request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getModelsMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Requests a new session with the given config and returns the session
     * ID, which is required for other rpc methods.
     * </pre>
     */
    public void newSession(cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.SessionID> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getNewSessionMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Terminates an existing session and closes any open event streams.
     * It is an error if the SessionEndRequest has an invalid SessionID.
     * </pre>
     */
    public void endSession(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getEndSessionMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Requests a new event stream for the given session.
     * </pre>
     */
    public void sessionEventStream(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent> responseObserver) {
      asyncServerStreamingCall(
          getChannel().newCall(getSessionEventStreamMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Notify Diatheke when a command has completed so that it may update
     * the dialog state. The initial command request will come as part of
     * a DiathekeEvent. While not strictly required (depeding on the model
     * and command), it is best practice to always call this method when a
     * command is complete. Cases where it is required include when the 
     * command has output parameters, or when the command is followed by 
     * another action in the Diatheke model.
     * </pre>
     */
    public void commandFinished(cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getCommandFinishedMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Begin an audio input stream for a session. The first message to
     * the server should specify the sessionID, with binary audio data pushed
     * for every subsequent message. As the audio is recognized, Diatheke
     * will respond with appropriate events on the session's event stream.
     * &lt;p&gt;
     * While it is allowed to call this multiple times during a single session,
     * clients should never have multiple audio input streams running concurrently
     * for the same session (the audio may mix and result in unpredictable
     * behavior). Previous audio streams should always be closed before starting
     * a new one.
     * </pre>
     */
    public io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.AudioInput> streamAudioInput(
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      return asyncClientStreamingCall(
          getChannel().newCall(getStreamAudioInputMethod(), getCallOptions()), responseObserver);
    }

    /**
     * <pre>
     * Create an audio reply stream for a session. The returned stream 
     * will receive replies ("say" entries in the Diatheke model) from the
     * server as they occur in the conversation. For each "say" entry, the 
     * stream will first receive the text to synthesize (defined by the model),
     * followed by one or more messages containing the synthesized audio bytes. 
     * The "say" entry will end with a message indicating that TTS for that 
     * entry is complete.
     * NOTE: The text in the first message of an audio reply is the same that
     *       will be received in the session's event stream.
     * </pre>
     */
    public void streamAudioReplies(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.AudioReply> responseObserver) {
      asyncServerStreamingCall(
          getChannel().newCall(getStreamAudioRepliesMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Push text to Diatheke as part of the conversation for a session.
     * Diatheke will respond with an appropriate event on the session's
     * event stream based on whether the given text was recognized as a
     * valid intent or not.
     * </pre>
     */
    public void pushText(cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPushTextMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Manually run streaming ASR unrelated to any session by pushing
     * audio data to the server on the audio stream. As transcriptions
     * become available, the server will return them on the ASRResponse
     * stream. The transcriptions may then be used for, e.g., the PushText
     * method. This function is provided as a convenience.
     * </pre>
     */
    public io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ASRRequest> streamASR(
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse> responseObserver) {
      return asyncBidiStreamingCall(
          getChannel().newCall(getStreamASRMethod(), getCallOptions()), responseObserver);
    }

    /**
     * <pre>
     * Manually run streaming TTS. The Audio stream will receive
     * binary audio data as it is synthesized and will close automatically
     * when synthesis is complete. This function is provided as a
     * convenience.
     * </pre>
     */
    public void streamTTS(cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest request,
        io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse> responseObserver) {
      asyncServerStreamingCall(
          getChannel().newCall(getStreamTTSMethod(), getCallOptions()), request, responseObserver);
    }
  }

  /**
   * <pre>
   * Service that implements the Cobalt Diatheke Dialog Management API.
   * </pre>
   */
  public static final class DiathekeBlockingStub extends io.grpc.stub.AbstractStub<DiathekeBlockingStub> {
    private DiathekeBlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private DiathekeBlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected DiathekeBlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new DiathekeBlockingStub(channel, callOptions);
    }

    /**
     * <pre>
     * Queries the Version of the Server.
     * </pre>
     */
    public cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse version(cobaltspeech.diatheke.DiathekeOuterClass.Empty request) {
      return blockingUnaryCall(
          getChannel(), getVersionMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Models will return a list of available versions.  Model values from
     * this list may be used in NewSession calls.
     * </pre>
     */
    public cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse models(cobaltspeech.diatheke.DiathekeOuterClass.Empty request) {
      return blockingUnaryCall(
          getChannel(), getModelsMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Requests a new session with the given config and returns the session
     * ID, which is required for other rpc methods.
     * </pre>
     */
    public cobaltspeech.diatheke.DiathekeOuterClass.SessionID newSession(cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest request) {
      return blockingUnaryCall(
          getChannel(), getNewSessionMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Terminates an existing session and closes any open event streams.
     * It is an error if the SessionEndRequest has an invalid SessionID.
     * </pre>
     */
    public cobaltspeech.diatheke.DiathekeOuterClass.Empty endSession(cobaltspeech.diatheke.DiathekeOuterClass.SessionID request) {
      return blockingUnaryCall(
          getChannel(), getEndSessionMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Requests a new event stream for the given session.
     * </pre>
     */
    public java.util.Iterator<cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent> sessionEventStream(
        cobaltspeech.diatheke.DiathekeOuterClass.SessionID request) {
      return blockingServerStreamingCall(
          getChannel(), getSessionEventStreamMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Notify Diatheke when a command has completed so that it may update
     * the dialog state. The initial command request will come as part of
     * a DiathekeEvent. While not strictly required (depeding on the model
     * and command), it is best practice to always call this method when a
     * command is complete. Cases where it is required include when the 
     * command has output parameters, or when the command is followed by 
     * another action in the Diatheke model.
     * </pre>
     */
    public cobaltspeech.diatheke.DiathekeOuterClass.Empty commandFinished(cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus request) {
      return blockingUnaryCall(
          getChannel(), getCommandFinishedMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Create an audio reply stream for a session. The returned stream 
     * will receive replies ("say" entries in the Diatheke model) from the
     * server as they occur in the conversation. For each "say" entry, the 
     * stream will first receive the text to synthesize (defined by the model),
     * followed by one or more messages containing the synthesized audio bytes. 
     * The "say" entry will end with a message indicating that TTS for that 
     * entry is complete.
     * NOTE: The text in the first message of an audio reply is the same that
     *       will be received in the session's event stream.
     * </pre>
     */
    public java.util.Iterator<cobaltspeech.diatheke.DiathekeOuterClass.AudioReply> streamAudioReplies(
        cobaltspeech.diatheke.DiathekeOuterClass.SessionID request) {
      return blockingServerStreamingCall(
          getChannel(), getStreamAudioRepliesMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Push text to Diatheke as part of the conversation for a session.
     * Diatheke will respond with an appropriate event on the session's
     * event stream based on whether the given text was recognized as a
     * valid intent or not.
     * </pre>
     */
    public cobaltspeech.diatheke.DiathekeOuterClass.Empty pushText(cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest request) {
      return blockingUnaryCall(
          getChannel(), getPushTextMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Manually run streaming TTS. The Audio stream will receive
     * binary audio data as it is synthesized and will close automatically
     * when synthesis is complete. This function is provided as a
     * convenience.
     * </pre>
     */
    public java.util.Iterator<cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse> streamTTS(
        cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest request) {
      return blockingServerStreamingCall(
          getChannel(), getStreamTTSMethod(), getCallOptions(), request);
    }
  }

  /**
   * <pre>
   * Service that implements the Cobalt Diatheke Dialog Management API.
   * </pre>
   */
  public static final class DiathekeFutureStub extends io.grpc.stub.AbstractStub<DiathekeFutureStub> {
    private DiathekeFutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private DiathekeFutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected DiathekeFutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new DiathekeFutureStub(channel, callOptions);
    }

    /**
     * <pre>
     * Queries the Version of the Server.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse> version(
        cobaltspeech.diatheke.DiathekeOuterClass.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getVersionMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Models will return a list of available versions.  Model values from
     * this list may be used in NewSession calls.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse> models(
        cobaltspeech.diatheke.DiathekeOuterClass.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getModelsMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Requests a new session with the given config and returns the session
     * ID, which is required for other rpc methods.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<cobaltspeech.diatheke.DiathekeOuterClass.SessionID> newSession(
        cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getNewSessionMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Terminates an existing session and closes any open event streams.
     * It is an error if the SessionEndRequest has an invalid SessionID.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<cobaltspeech.diatheke.DiathekeOuterClass.Empty> endSession(
        cobaltspeech.diatheke.DiathekeOuterClass.SessionID request) {
      return futureUnaryCall(
          getChannel().newCall(getEndSessionMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Notify Diatheke when a command has completed so that it may update
     * the dialog state. The initial command request will come as part of
     * a DiathekeEvent. While not strictly required (depeding on the model
     * and command), it is best practice to always call this method when a
     * command is complete. Cases where it is required include when the 
     * command has output parameters, or when the command is followed by 
     * another action in the Diatheke model.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<cobaltspeech.diatheke.DiathekeOuterClass.Empty> commandFinished(
        cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus request) {
      return futureUnaryCall(
          getChannel().newCall(getCommandFinishedMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Push text to Diatheke as part of the conversation for a session.
     * Diatheke will respond with an appropriate event on the session's
     * event stream based on whether the given text was recognized as a
     * valid intent or not.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<cobaltspeech.diatheke.DiathekeOuterClass.Empty> pushText(
        cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getPushTextMethod(), getCallOptions()), request);
    }
  }

  private static final int METHODID_VERSION = 0;
  private static final int METHODID_MODELS = 1;
  private static final int METHODID_NEW_SESSION = 2;
  private static final int METHODID_END_SESSION = 3;
  private static final int METHODID_SESSION_EVENT_STREAM = 4;
  private static final int METHODID_COMMAND_FINISHED = 5;
  private static final int METHODID_STREAM_AUDIO_REPLIES = 6;
  private static final int METHODID_PUSH_TEXT = 7;
  private static final int METHODID_STREAM_TTS = 8;
  private static final int METHODID_STREAM_AUDIO_INPUT = 9;
  private static final int METHODID_STREAM_ASR = 10;

  private static final class MethodHandlers<Req, Resp> implements
      io.grpc.stub.ServerCalls.UnaryMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ServerStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ClientStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.BidiStreamingMethod<Req, Resp> {
    private final DiathekeImplBase serviceImpl;
    private final int methodId;

    MethodHandlers(DiathekeImplBase serviceImpl, int methodId) {
      this.serviceImpl = serviceImpl;
      this.methodId = methodId;
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public void invoke(Req request, io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_VERSION:
          serviceImpl.version((cobaltspeech.diatheke.DiathekeOuterClass.Empty) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.VersionResponse>) responseObserver);
          break;
        case METHODID_MODELS:
          serviceImpl.models((cobaltspeech.diatheke.DiathekeOuterClass.Empty) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ModelsResponse>) responseObserver);
          break;
        case METHODID_NEW_SESSION:
          serviceImpl.newSession((cobaltspeech.diatheke.DiathekeOuterClass.NewSessionRequest) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.SessionID>) responseObserver);
          break;
        case METHODID_END_SESSION:
          serviceImpl.endSession((cobaltspeech.diatheke.DiathekeOuterClass.SessionID) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty>) responseObserver);
          break;
        case METHODID_SESSION_EVENT_STREAM:
          serviceImpl.sessionEventStream((cobaltspeech.diatheke.DiathekeOuterClass.SessionID) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.DiathekeEvent>) responseObserver);
          break;
        case METHODID_COMMAND_FINISHED:
          serviceImpl.commandFinished((cobaltspeech.diatheke.DiathekeOuterClass.CommandStatus) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty>) responseObserver);
          break;
        case METHODID_STREAM_AUDIO_REPLIES:
          serviceImpl.streamAudioReplies((cobaltspeech.diatheke.DiathekeOuterClass.SessionID) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.AudioReply>) responseObserver);
          break;
        case METHODID_PUSH_TEXT:
          serviceImpl.pushText((cobaltspeech.diatheke.DiathekeOuterClass.PushTextRequest) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty>) responseObserver);
          break;
        case METHODID_STREAM_TTS:
          serviceImpl.streamTTS((cobaltspeech.diatheke.DiathekeOuterClass.TTSRequest) request,
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.TTSResponse>) responseObserver);
          break;
        default:
          throw new AssertionError();
      }
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public io.grpc.stub.StreamObserver<Req> invoke(
        io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_STREAM_AUDIO_INPUT:
          return (io.grpc.stub.StreamObserver<Req>) serviceImpl.streamAudioInput(
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.Empty>) responseObserver);
        case METHODID_STREAM_ASR:
          return (io.grpc.stub.StreamObserver<Req>) serviceImpl.streamASR(
              (io.grpc.stub.StreamObserver<cobaltspeech.diatheke.DiathekeOuterClass.ASRResponse>) responseObserver);
        default:
          throw new AssertionError();
      }
    }
  }

  private static volatile io.grpc.ServiceDescriptor serviceDescriptor;

  public static io.grpc.ServiceDescriptor getServiceDescriptor() {
    io.grpc.ServiceDescriptor result = serviceDescriptor;
    if (result == null) {
      synchronized (DiathekeGrpc.class) {
        result = serviceDescriptor;
        if (result == null) {
          serviceDescriptor = result = io.grpc.ServiceDescriptor.newBuilder(SERVICE_NAME)
              .addMethod(getVersionMethod())
              .addMethod(getModelsMethod())
              .addMethod(getNewSessionMethod())
              .addMethod(getEndSessionMethod())
              .addMethod(getSessionEventStreamMethod())
              .addMethod(getCommandFinishedMethod())
              .addMethod(getStreamAudioInputMethod())
              .addMethod(getStreamAudioRepliesMethod())
              .addMethod(getPushTextMethod())
              .addMethod(getStreamASRMethod())
              .addMethod(getStreamTTSMethod())
              .build();
        }
      }
    }
    return result;
  }
}
