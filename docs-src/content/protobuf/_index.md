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
| Version | [Empty](#Empty) | [VersionResponse](#VersionResponse) | Returns version information from the server. |
| ListModels | [Empty](#Empty) | [ListModelsResponse](#ListModelsResponse) | ListModels returns information about the Diatheke models the server can access. |
| CreateSession | [SessionStart](#SessionStart) | [SessionOutput](#SessionOutput) | Create a new Diatheke session. Also returns a list of actions to take next. |
| DeleteSession | [TokenData](#TokenData) | [Empty](#Empty) | Delete the session. Behavior is undefined if the given TokenData is used again after this function is called. |
| UpdateSession | [SessionInput](#SessionInput) | [SessionOutput](#SessionOutput) | Process input for a session and get an updated session with a list of actions to take next. This is the only method that modifies the Diatheke session state. |
| StreamASR | [ASRInput](#ASRInput) | [ASRResult](#ASRResult) | Create an ASR stream. A result is returned when the stream is closed by the client (which forces the ASR to endpoint), or when a transcript becomes available on its own, in which case the stream is closed by the server. The ASR result may be used in the UpdateSession method. <br/><br/> If the session has a wakeword enabled, and the client application is using Diatheke and Cubic to handle the wakeword processing, this method will not return a result until the wakeword condition has been satisfied. Utterances without the required wakeword will be discarded and no transcription will be returned. |
| StreamTTS | [ReplyAction](#ReplyAction) | [TTSAudio](#TTSAudio) | Create a TTS stream to receive audio for the given reply. The stream will close when TTS is finished. The client may also close the stream early to cancel the speech synthesis. |
| Transcribe | [TranscribeInput](#TranscribeInput) | [TranscribeResult](#TranscribeResult) | Create an ASR stream for transcription. Unlike StreamASR, Transcribe does not listen for a wakeword. This method returns a bi-directional stream, and its intended use is for situations where a user may say anything at all, whether it is short or long, and the application wants to save the transcript (e.g., take a note, send a message). <br/><br/> The first message sent to the server must be the TranscribeAction, with remaining messages sending audio data. Messages received from the server will include the current best partial transcription until the full transcription is ready. The stream ends when either the client application closes it, a predefined duration of silence (non-speech) occurs, or the end-transcription intent is recognized. |

 <!-- end services -->



<a name="ASRInput"></a>
### Message: ASRInput
Data to send to the ASR stream. The first message on the
stream must be the session token followed by audio data.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| token | [TokenData](#TokenData) |  | <p>Session data, used to determine the correct Cubic model to use for ASR, with other contextual information.</p> |
| audio | [bytes](#bytes) |  | <p>Audio data to transcribe.</p> |







<a name="ASRResult"></a>
### Message: ASRResult
The result from the ASR stream, sent after the ASR engine
has endpointed or the stream was closed by the client.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p>The transcription.</p> |
| confidence | [double](#double) |  | <p>Confidence estimate between 0 and 1. A higher number represents a higher likelihood of the output being correct.</p> |
| timedOut | [bool](#bool) |  | <p>True if a timeout was defined for the session's current input state in the Diatheke model, and the timeout expired before getting a transcription. This timeout refers to the amount of time a user has to verbally respond to Diatheke after the ASR stream has been created, and should not be confused with a network connection timeout.</p> |







<a name="ActionData"></a>
### Message: ActionData
Specifies an action that the client application should take.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| input | [WaitForUserAction](#WaitForUserAction) |  | <p>The user must provide input to Diatheke.</p> |
| command | [CommandAction](#CommandAction) |  | <p>The client app must execute the specified command.</p> |
| reply | [ReplyAction](#ReplyAction) |  | <p>The client app should provide the reply to the user.</p> |
| transcribe | [TranscribeAction](#TranscribeAction) |  | <p>The client app should call the Transcribe method to capture the user's input.</p> |







<a name="CommandAction"></a>
### Message: CommandAction
This action indicates that the client application should
execute a command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| id | [string](#string) |  | <p>The ID of the command to execute, as defined in the Diatheke model.</p> |
| input_parameters | [CommandAction.InputParametersEntry](#CommandAction.InputParametersEntry) | repeated | <p></p> |







<a name="CommandAction.InputParametersEntry"></a>
### Message: CommandAction.InputParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | [string](#string) |  | <p></p> |
| value | [string](#string) |  | <p></p> |







<a name="CommandResult"></a>
### Message: CommandResult
The result of executing a command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| id | [string](#string) |  | <p>The command ID, as given by the CommandAction</p> |
| out_parameters | [CommandResult.OutParametersEntry](#CommandResult.OutParametersEntry) | repeated | <p>Output from the command expected by the Diatheke model. For example, this could be the result of a data query.</p> |
| error | [string](#string) |  | <p>If there was an error during execution, indicate it here with a brief message that will be logged by Diatheke.</p> |







<a name="CommandResult.OutParametersEntry"></a>
### Message: CommandResult.OutParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | [string](#string) |  | <p></p> |
| value | [string](#string) |  | <p></p> |







<a name="Empty"></a>
### Message: Empty



This message is empty and has no fields.






<a name="ListModelsResponse"></a>
### Message: ListModelsResponse
A list of models available on the Diatheke server.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| models | [ModelInfo](#ModelInfo) | repeated | <p></p> |







<a name="ModelInfo"></a>
### Message: ModelInfo
Information about a single Diatheke model.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| id | [string](#string) |  | <p>Diatheke model ID, which is used to create a new session.</p> |
| name | [string](#string) |  | <p>Pretty model name, which may be used for display purposes.</p> |
| language | [string](#string) |  | <p>Language code of the model.</p> |
| asr_sample_rate | [uint32](#uint32) |  | <p>The ASR audio sample rate, if ASR is enabled.</p> |
| tts_sample_rate | [uint32](#uint32) |  | <p>The TTS audio sample rate, if TTS is enabled.</p> |







<a name="ReplyAction"></a>
### Message: ReplyAction
This action indicates that the client application should
give the provided text to the user. This action may also 
be used to synthesize speech with the StreamTTS method.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p>Text of the reply</p> |
| luna_model | [string](#string) |  | <p>TTS model to use with the TTSReply method</p> |







<a name="SessionInput"></a>
### Message: SessionInput
Used by Diatheke to update the session state.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| token | [TokenData](#TokenData) |  | <p>The session token.</p> |
| text | [TextInput](#TextInput) |  | <p>Process the user supplied text.</p> |
| asr | [ASRResult](#ASRResult) |  | <p>Process an ASR result.</p> |
| cmd | [CommandResult](#CommandResult) |  | <p>Process the result of a completed command.</p> |
| story | [SetStory](#SetStory) |  | <p>Change the current session state.</p> |







<a name="SessionOutput"></a>
### Message: SessionOutput
The result of updating a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| token | [TokenData](#TokenData) |  | <p>The updated session token.</p> |
| action_list | [ActionData](#ActionData) | repeated | <p>The list of actions the client should take next, using the session token returned with this result.</p> |







<a name="SessionStart"></a>
### Message: SessionStart
Used to create a new session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model_id | [string](#string) |  | <p>Specifies the Diatheke model ID to use for the session.</p> |
| wakeword | [string](#string) |  | <p>Specifies a custom wakeword to use for this session. The wakeword must be enabled in the Diatheke model for this to have any effect. It will override the default wakeword specified in the model.</p> |







<a name="SetStory"></a>
### Message: SetStory
Changes the current state of a Diatheke session to run at
the specified story.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| story_id | [string](#string) |  | <p>The ID of the story to run, as defined in the Diatheke model.</p> |
| parameters | [SetStory.ParametersEntry](#SetStory.ParametersEntry) | repeated | <p>A list of parameters to set before running the given story. This will replace any parameters currently defined in the session.</p> |







<a name="SetStory.ParametersEntry"></a>
### Message: SetStory.ParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | [string](#string) |  | <p></p> |
| value | [string](#string) |  | <p></p> |







<a name="TTSAudio"></a>
### Message: TTSAudio
Contains synthesized speech audio. The specific encoding
is defined in the server config file.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| audio | [bytes](#bytes) |  | <p></p> |







<a name="TextInput"></a>
### Message: TextInput
User supplied text to send to Diatheke for processing.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p></p> |







<a name="TokenData"></a>
### Message: TokenData
A token that represents a single Diatheke session and its
current state.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| data | [bytes](#bytes) |  | <p></p> |
| id | [string](#string) |  | <p>Session ID, useful for correlating logging between a client and the server.</p> |
| metadata | [string](#string) |  | <p>Additional data supplied by the client app, which will be logged with other session info by the server.</p> |







<a name="TranscribeAction"></a>
### Message: TranscribeAction
This action indicates that the client application should
call the Transcribe method to capture the user's input.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| id | [string](#string) |  | <p>The ID of the transcribe action, which is useful to differentiate separate transcription tasks within a single sesssion.</p> |
| cubic_model_id | [string](#string) |  | <p>(Required) The ASR model to use for transcription.</p> |
| diatheke_model_id | [string](#string) |  | <p>(Optional) A Diatheke model to use for end-of-stream conditions. If empty, the server will not be able to automatically close the transcribe stream based on conditions defined in the model, such as a non-speech timeout or an "end-transcription" intent. When empty, the stream must be closed by the client application.</p> |







<a name="TranscribeInput"></a>
### Message: TranscribeInput
Data to send to the Transcribe stream. The first message on
the stream must be a TranscribeAction, followed by audio data.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| action | [TranscribeAction](#TranscribeAction) |  | <p>Action defining the transcribe configuration.</p> |
| audio | [bytes](#bytes) |  | <p>Audio data to transcribe.</p> |







<a name="TranscribeResult"></a>
### Message: TranscribeResult
The result from the Transcribe stream. Usually, several partial
(or intermediate) transcriptions will be sent until the final
transcription is ready for every utterance processed.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | [string](#string) |  | <p>The transcription.</p> |
| confidence | [double](#double) |  | <p>Confidence estimate between 0 and 1. A higher number represents a higher likelihood that the transcription is correct.</p> |
| is_partial | [bool](#bool) |  | <p>True if this is a partial result, in which case the next result will be for the same audio, either repeating or correcting the text in this result. When false, this represents the final transcription for an utterance, which will not change with further audio input. It is sent when the ASR has identified an endpoint. After the final transcription is sent, any additional results sent on the Transcribe stream belong to the next utterance.</p> |







<a name="VersionResponse"></a>
### Message: VersionResponse
Lists the version of Diatheke and the engines it uses.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| diatheke | [string](#string) |  | <p>Dialog management engine</p> |
| chosun | [string](#string) |  | <p>NLU engine</p> |
| cubic | [string](#string) |  | <p>ASR engine</p> |
| luna | [string](#string) |  | <p>TTS engine</p> |







<a name="WaitForUserAction"></a>
### Message: WaitForUserAction
This action indicates that Diatheke is expecting user input.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| requires_wake_word | [bool](#bool) |  | <p>True if the next user input must begin with a wake-word.</p> |
| immediate | [bool](#bool) |  | <p>True if the input is required immediately (i.e., in response to a question Diatheke asked the user). When false, the client should be allowed to wait indefinitely for the user to provide input.</p> |





 <!-- end messages -->

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


