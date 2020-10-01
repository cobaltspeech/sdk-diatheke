---
title: "Diatheke API Reference"
weight: 1000
---

The Diatheke API is defined using [gRPC and protocol buffers](https://grpc.io/docs/guides/).
This section of the documentation is auto-generated from the 
[protobuf file](https://github.com/cobaltspeech/sdk-diatheke/blob/master/grpc/diatheke.proto).
It describes the data types and functions defined in the spec. The "messages" below
correspond to the data structures to be used, and the "service" contains the
methods that can be called.




## diatheke.proto





### Service: Diatheke
Service that implements the Cobalt Diatheke Dialog Management API.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| Version | [Empty](#Empty) | [VersionResponse](#VersionResponse) | Queries the Version of the Server. |
| Models | [Empty](#Empty) | [ModelsResponse](#ModelsResponse) | Models will return a list of available versions. Model values from this list may be used in NewSession calls. |
| NewSession | [NewSessionRequest](#NewSessionRequest) | [SessionID](#SessionID) | Requests a new session with the given config and returns the session ID, which is required for other rpc methods. After the session is created, StartSession() must be called to begin executing the Diatheke model. |
| StartSession | [SessionID](#SessionID) | [Empty](#Empty) | Begin execution of the model for the given session ID. The session's event stream should be set up prior to calling this function so that the client application can respond to any initialization events that are defined in the session's model. |
| EndSession | [SessionID](#SessionID) | [Empty](#Empty) | Terminates an existing session and closes any open session streams. It is an error if the SessionEndRequest has an invalid SessionID. |
| SessionEventStream | [SessionID](#SessionID) | [DiathekeEvent](#DiathekeEvent) | Requests a new event stream for the given session. Only one stream per session is allowed. |
| CommandFinished | [CommandStatus](#CommandStatus) | [Empty](#Empty) | Notify Diatheke when a command has completed so that it may update the dialog state. The initial command request will come as part of a DiathekeEvent. After sending a CommandEvent, Diatheke will wait until it receives the CommandFinished notification before continuing to the next action in the model. Client applications should therefore always call this after receiving a CommandEvent, or else the session will hang. |
| StreamAudioInput | [AudioInput](#AudioInput) | [Empty](#Empty) | Begin an audio input stream for a session. The first message to the server should specify the sessionID, with binary audio data pushed for every subsequent message. As the audio is recognized, Diatheke will respond with appropriate events on the session's event stream. <p> Only one stream at a time is allowed for a session. A previously created audio input stream must be closed before starting a new one. |
| StreamAudioReplies | [SessionID](#SessionID) | [AudioReply](#AudioReply) | Create an audio reply stream for a session. The returned stream will receive replies (as defined in the Diatheke model) from the server as they occur in the conversation. For each reply, the stream will first receive the text to synthesize (defined by the model), followed by one or more messages containing the synthesized audio bytes. The reply will end with a message indicating that TTS for that entry is complete. Only one reply stream at a time is allowed for a session. NOTE: The text in the first message of an audio reply is the same that will be received in the session's event stream. |
| PushText | [PushTextRequest](#PushTextRequest) | [Empty](#Empty) | Push text to Diatheke as part of the conversation for a session. Diatheke will respond with an appropriate event on the session's event stream based on whether the given text was recognized as a valid intent or not. |
| SetStory | [StoryRequest](#StoryRequest) | [Empty](#Empty) | Set the current story for a running session. This function can be used to implement system initiated alerts or to change the current session state. Events for the new story will come over the session's event stream. |
| StreamASR | [ASRRequest](#ASRRequest) | [ASRResponse](#ASRResponse) | Manually run streaming ASR unrelated to any session by pushing audio data to the server on the audio stream. As transcriptions become available, the server will return them on the ASRResponse stream. The transcriptions may then be used for, e.g., the PushText method. This function is provided as a convenience. |
| StreamTTS | [TTSRequest](#TTSRequest) | [TTSResponse](#TTSResponse) | Manually run streaming TTS. The Audio stream will receive binary audio data as it is synthesized and will close automatically when synthesis is complete. This function is provided as a convenience. |

 <!-- end services -->



<a name="ASRRequest"></a>
### Message: ASRRequest
Request for streaming ASR unrelated to a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | [string](#string) |  | <p>The Cubic model to use for ASR. This message should always be sent before any audio data is sent.</p> |
| audio | [bytes](#bytes) |  | <p>Audio data to process. The encoding of the data should match what was specified in the Diatheke server configuration. NOTE: If the audio data is empty, the server may interpret it as the end of the stream and stop accepting further messages.</p> |







<a name="ASRResponse"></a>
### Message: ASRResponse
ASRResponse contains speech recognition results.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p>Text is the Cubic engine's formatted transcript of pushed audio. This field will be the 1-best alternative.</p> |
| confidence_score | [double](#double) |  | <p>The confidence score is a floating point number between 0.0 - 1.0. A score of 1.0 indicates that the ASR engine is 100% confident in the transcription.</p> |







<a name="AtStartEvent"></a>
### Message: AtStartEvent
The AtStartEvent is sent when a Diatheke session returns back the
start state of the model.


This message is empty and has no fields.






<a name="AudioInput"></a>
### Message: AudioInput
Provides input audio data for StreamAudioInput. The first message
sent must contain the session ID only. All subsequent messages
must contain audio data only.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | [string](#string) |  | <p>Session ID returned from the NewSession call.</p> |
| data | [bytes](#bytes) |  | <p>Audio data to process. The encoding of the data should match what was specified in the Diatheke server configuration. NOTE: If the audio data is empty, the server may interpret it as the end of the stream and stop accepting further messages.</p> |







<a name="AudioReply"></a>
### Message: AudioReply
An AudioReply is the verbal and textual reply that Diatheke returns as
part of a conversation (not to be confused with the server concepts of
request and response).


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| label | [string](#string) |  | <p>The label defined in the Diatheke model. Identifies which reply in the model this message corresponds to.</p> |
| text | [string](#string) |  | <p>The reply text as defined in the Diatheke model. This is the first message that will be received for an AudioReply. It contains the same text as the corresponding ReplyEvent in the session's event stream.</p> |
| data | [bytes](#bytes) |  | <p>The audio data from TTS. There can be any number of these messages for an AudioReply after the first text message and before the final end message. The encoding of the data will match what was specified in the server configuration.</p> |
| end | [Empty](#Empty) |  | <p>Indicates that TTS has finished streaming audio for the reply. This is the last message that will be received for an AudioReply.</p> |







<a name="CommandEvent"></a>
### Message: CommandEvent
A CommandEvent occurs when Diatheke wants the client to execute the
given command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| command_id | [string](#string) |  | <p>ID of the command that should be run. i.e. "COM01" for Command #01.</p> |
| parameters | [CommandEvent.ParametersEntry](#CommandEvent.ParametersEntry) | repeated | <p>A generic map of parameters (name, value). The parameters are defined in the Diatheke model. Depending on the command, these parameters should be sent back with the CommandStatus update.</p> |
| command_state_id | [string](#string) |  | <p>ID to keep track of the dialog state when the command is requested. This field is required in the CommandStatus message so that Diatheke can correctly update the dialog state when CommandFinished is called.</p> |







<a name="CommandEvent.ParametersEntry"></a>
### Message: CommandEvent.ParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | [string](#string) |  | <p></p> |
| value | [string](#string) |  | <p></p> |







<a name="CommandStatus"></a>
### Message: CommandStatus
The final status of an executed command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | [string](#string) |  | <p>session_id should be the same as the status id returned from NewSessionResponse.</p> |
| command_id | [string](#string) |  | <p>ID of the command as given in the RunCommand object.</p> |
| return_status | [CommandStatus.StatusCode](#CommandStatus.StatusCode) |  | <p></p> |
| output_parameters | [CommandStatus.OutputParametersEntry](#CommandStatus.OutputParametersEntry) | repeated | <p>Parameters to return to Diatheke. For example, the map might contain the entry "temperature", which was populated with a value of "30" after the command finished. Expected parameters are defined by the Diatheke model.</p> |
| error_message_text | [string](#string) |  | <p>Set this field with an error message if a fatal error occured while executing the command (return_status == FAILURE).</p> |
| command_state_id | [string](#string) |  | <p>State ID from the original CommandEvent. This field is required for Diatheke to correctly update the dialog state when CommandFinished is called.</p> |







<a name="CommandStatus.OutputParametersEntry"></a>
### Message: CommandStatus.OutputParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | [string](#string) |  | <p></p> |
| value | [string](#string) |  | <p></p> |







<a name="DiathekeEvent"></a>
### Message: DiathekeEvent
An event from Diatheke in response to either recognized audio,
submitted text, or some other transition in the model.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| command | [CommandEvent](#CommandEvent) |  | <p>Indicates Diatheke found an actionable state in the dialog, and requests the client to perform the given command.</p><p>Users should always call CommandFinished after receiving this event so that Diatheke can update the dialog state when the command is complete.</p> |
| recognize | [RecognizeEvent](#RecognizeEvent) |  | <p>An event indicating whether pushed text and audio was recognized by ASR and/or Diatheke.</p> |
| reply | [ReplyEvent](#ReplyEvent) |  | <p>The textual reply from Diatheke in the conversation (not to be confused with the server concepts of request and response). For example, this could be a question to solicit more information from the user, a status report, or any other reply defined by the Diatheke model. The text of this message is also provided in the AudioReply stream (if one is open).</p> |
| input_required | [InputRequiredEvent](#InputRequiredEvent) |  | <p>Indicates that Diatheke is expecting user input (text or audio), which is defined by input actions in the Diatheke model.</p> |
| at_start | [AtStartEvent](#AtStartEvent) |  | <p>Indicates that Diatheke has returned to the start state of the model.</p> |







<a name="Empty"></a>
### Message: Empty



This message is empty and has no fields.






<a name="InputRequiredEvent"></a>
### Message: InputRequiredEvent
An InputRequiredEvent occurs when Diatheke is expecting input from
the user (text or audio).


This message is empty and has no fields.






<a name="ModelsResponse"></a>
### Message: ModelsResponse
The message sent by the server in response to a Models request.
Returns an array of model names.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| models | [string](#string) | repeated | <p>Array of models available for use.</p> |







<a name="NewSessionRequest"></a>
### Message: NewSessionRequest
Request for the NewSession call.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | [string](#string) |  | <p>For applications that have more than one model to use for ASR/NLU. ASR grammar can vary between models, as well as sets of commands. Some applications will only have one model.</p> |







<a name="PushTextRequest"></a>
### Message: PushTextRequest
Request to push text to Diatheke as part of a conversation.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | [string](#string) |  | <p>Session ID returned from the NewSession call.</p> |
| text | [string](#string) |  | <p>User input. This could be a transcription from manually run ASR, text selected from a dropdown list, entered in a prompt, etc.</p> |







<a name="RecognizeEvent"></a>
### Message: RecognizeEvent
A RecognizeEvent occurs if a session's audio input has a transcription
available, or if the PushText method was called. In both cases, the
event will indicate whether the text was recognized as a valid intent
by the Diatheke model.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p>The pushed text or transcription of audio sent to Diatheke.</p> |
| valid_input | [bool](#bool) |  | <p>True if the submitted text or audio transcription was recognized by the Diatheke model as a valid intent or entity.</p> |







<a name="ReplyEvent"></a>
### Message: ReplyEvent
A ReplyEvent occurs when Diatheke has a reply in the conversation (not
to be confused with the server concepts of request and response). These
correspond to replies defined in the Diatheke model. For example, it might
be a prompt for additional information from the user, a status update,
or a confirmation. ReplyEvents are not generated in response to 
StreamTTS calls.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p>Text of the reply event (defined by the Diatheke model).</p> |
| label | [string](#string) |  | <p>Label of the reply event (defined by the Diatheke model).</p> |







<a name="SessionID"></a>
### Message: SessionID
Simple message that only contains the session ID.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | [string](#string) |  | <p>Session ID returned from the NewSession call.</p> |







<a name="StoryRequest"></a>
### Message: StoryRequest
Request to change the current story of a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | [string](#string) |  | <p>ID of the session that will have its story changed.</p> |
| story_id | [string](#string) |  | <p>ID of the story to switch to. This ID is defined by the model used to create the session.</p> |
| parameters | [StoryRequest.ParametersEntry](#StoryRequest.ParametersEntry) | repeated | <p>Parameters to set in session memory before executing the specified story. Some stories in the model may make assumptions about which parameters have already been defined, so it is important to be familiar with the model requirements for any given story.</p> |
| wait_for_start | [bool](#bool) |  | <p>If true, the given story will not be executed until the session completes the current stories and returns back to the main story. If false, the current story in the session will be immediately interrupted to execute the specified story.</p> |
| temporary | [bool](#bool) |  | <p>If true, once the given story has finished, Diatheke will return the session to the place in the model where it was when this request was received, and restore the parameters that were defined at that time. This is useful when the change in story represents a temporary interruption. If false, Diatheke will simply continue from the given story without trying to go back to its prior state, which is useful to make a permanent state change.</p> |







<a name="StoryRequest.ParametersEntry"></a>
### Message: StoryRequest.ParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | [string](#string) |  | <p></p> |
| value | [string](#string) |  | <p></p> |







<a name="TTSRequest"></a>
### Message: TTSRequest
Request to synthesize speech unrelated to a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | [string](#string) |  | <p>The Luna model to use for TTS (defined in the server config file).</p> |
| text | [string](#string) |  | <p>Text to synthesize</p> |







<a name="TTSResponse"></a>
### Message: TTSResponse
Response for text-to-speech unrelated to a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| data | [bytes](#bytes) |  | <p>The synthesized audio data. The data encoding will match what was specified in the server configuration.</p> |







<a name="VersionResponse"></a>
### Message: VersionResponse
The message sent by the server for the `Version` method.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| server | [string](#string) |  | <p>Server that manages all of the the other components.</p> |





 <!-- end messages -->



<a name="CommandStatus.StatusCode"></a>
### Enum: CommandStatus.StatusCode
CommandStatus are the resulting states of a command.

| Name | Number | Description |
| ---- | ------ | ----------- |
| SUCCESS | 0 | SUCCESS indicates that the command was successfully completed, and the dialog state may now move on to the next state. |
| FAILURE | 1 | FAILURE indicates that there was a fatal error running the command. The session will log an error and return to the start state of the model when this status is encountered. |


 <!-- end enums -->

 <!-- end HasExtensions -->



## Well-Known Types
See the protocol buffer documentation for these

| .proto Type | Notes | 
| ----------- | ----- |
| <a name=".google.protobuf.Empty" href="https://developers.google.com/protocol-buffers/docs/reference/google.protobuf#duration">Duration</a> | Represents a signed, fixed-length span of time represented as a count of seconds and fractions of seconds at nanosecond resolution |
| <a name=".google.protobuf.Empty" href="https://developers.google.com/protocol-buffers/docs/reference/google.protobuf#empty">Empty</a> | Used to indicate a method takes or returns nothing |

## Scalar Value Types

| .proto Type | Notes | Go Type | Python Type | C++ Type |
| ----------- | ----- | ------- | ----------- | -------- |
| <a name="double"> </a> double |  | float64 | float | double |
| <a name="float"> </a> float |  | float32 | float | float |
| <a name="int32"> </a> int32 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint32 instead. | int32 | int | int32 |
| <a name="int64"> </a> int64 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint64 instead. | int64 | int/long | int64 |
| <a name="uint32"> </a> uint32 | Uses variable-length encoding. | uint32 | int/long | uint32 |
| <a name="uint64"> </a> uint64 | Uses variable-length encoding. | uint64 | int/long | uint64 |
| <a name="sint32"> </a> sint32 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int32s. | int32 | int | int32 |
| <a name="sint64"> </a> sint64 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int64s. | int64 | int/long | int64 |
| <a name="fixed32"> </a> fixed32 | Always four bytes. More efficient than uint32 if values are often greater than 2^28. | uint32 | int | uint32 |
| <a name="fixed64"> </a> fixed64 | Always eight bytes. More efficient than uint64 if values are often greater than 2^56. | uint64 | int/long | uint64 |
| <a name="sfixed32"> </a> sfixed32 | Always four bytes. | int32 | int | int32 |
| <a name="sfixed64"> </a> sfixed64 | Always eight bytes. | int64 | int/long | int64 |
| <a name="bool"> </a> bool |  | bool | boolean | bool |
| <a name="string"> </a> string | A string must always contain UTF-8 encoded or 7-bit ASCII text. | string | str/unicode | string |
| <a name="bytes"> </a> bytes | May contain any arbitrary sequence of bytes. | []byte | str | string |


