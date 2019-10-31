---
title: "Diatheke Protobuf API Docs"
weight: 100
---




## diatheke.proto





### Service: Diatheke
Service that implements the Cobalt Diatheke Dialog Management API.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| Version | Empty | VersionResponse | Queries the Version of the Server. |
| Models | Empty | ModelsResponse | Models will return a list of available versions. Model values from this list may be used in NewSession calls. |
| NewSession | NewSessionRequest | SessionID | Requests a new session with the given config and returns the session ID, which is required for other rpc methods. |
| EndSession | SessionID | Empty | Terminates an existing session and closes any open event streams. It is an error if the SessionEndRequest has an invalid SessionID. |
| SessionEventStream | SessionID | DiathekeEvent | Requests a new event stream for the given session. |
| CommandFinished | CommandStatus | Empty | Notify Diatheke when a command has completed so that it may update the dialog state. The initial command request will come as part of a DiathekeEvent. While not strictly required (depeding on the model and command), it is best practice to always call this method when a command is complete. Cases where it is required include when the command has output parameters, or when the command is followed by another action in the Diatheke model. |
| StreamAudioInput | AudioInput | Empty | Begin an audio input stream for a session. The first message to the server should specify the sessionID, with binary audio data pushed for every subsequent message. As the audio is recognized, Diatheke will respond with appropriate events on the session's event stream. <br> While it is allowed to call this multiple times during a single session, clients should never have multiple audio input streams running concurrently for the same session (the audio may mix and result in unpredictable behavior). Previous audio streams should always be closed before starting a new one. |
| StreamAudioReplies | SessionID | AudioReply | Create an audio reply stream for a session. The returned stream will receive replies ("say" entries in the Diatheke model) from the server as they occur in the conversation. For each "say" entry, the stream will first receive the text to synthesize (defined by the model), followed by one or more messages containing the synthesized audio bytes. The "say" entry will end with a message indicating that TTS for that entry is complete. NOTE: The text in the first message of an audio reply is the same that will be received in the session's event stream. |
| PushText | PushTextRequest | Empty | Push text to Diatheke as part of the conversation for a session. Diatheke will respond with an appropriate event on the session's event stream based on whether the given text was recognized as a valid intent or not. |
| StreamASR | ASRRequest | ASRResponse | Manually run streaming ASR unrelated to any session by pushing audio data to the server on the audio stream. As transcriptions become available, the server will return them on the ASRResponse stream. The transcriptions may then be used for, e.g., the PushText method. This function is provided as a convenience. |
| StreamTTS | TTSRequest | TTSResponse | Manually run streaming TTS. The Audio stream will receive binary audio data as it is synthesized and will close automatically when synthesis is complete. This function is provided as a convenience. |

 <!-- end services -->



### Message: ASRRequest
Request for streaming ASR unrelated to a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | string |  | <p>The model to use for ASR. This message should always be sent before any audio data is sent.</p> |
| audio | bytes |  | <p>Audio data to process. The encoding of the data should match what was specified in the Diatheke server configuration. NOTE: If the audio data is empty, the server may interpret it as the end of the stream and stop accepting further messages.</p> |







### Message: ASRResponse
ASRResponse contains speech recognition results.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | string |  | <p>Text is the Cubic engine's formatted transcript of pushed audio. This field will be the 1-best alternative.</p> |
| confidence_score | double |  | <p>The confidence score is a floating point number between 0.0 - 1.0. A score of 1.0 indicates that the ASR engine is 100% confident in the transcription.</p> |







### Message: AudioInput
Provides input audio data for StreamAudioInput. The first message
sent must contain the session ID only. All subsequent messages
must contain audio data only.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |
| data | bytes |  | <p>Audio data to process. The encoding of the data should match what was specified in the Diatheke server configuration. NOTE: If the audio data is empty, the server may interpret it as the end of the stream and stop accepting further messages.</p> |







### Message: AudioReply
An AudioReply is the verbal and textual reply that Diatheke returns as
part of a conversation (not to be confused with the server concepts of
request and response).


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| label | string |  | <p>The label defined in the Diatheke model. Identifies which "say" entry in the model this message corresponds to.</p> |
| text | string |  | <p>The reply text as defined in the Diatheke model. This is the first message that will be received for an AudioReply. It contains the same text as the corresponding ReplyEvent in the session's event stream.</p> |
| data | bytes |  | <p>The audio data from TTS. There can be any number of these messages for an AudioReply after the first text message and before the final end message. The encoding of the data will match what was specified in the server configuration.</p> |
| end | Empty |  | <p>Indicates that TTS has finished streaming audio for the reply. This is the last message that will be received for an AudioReply.</p> |







### Message: CommandEvent
A CommandEvent occurs when Diatheke wants the client to execute the
given command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| command_id | string |  | <p>ID of the command that should be run. i.e. "COM01" for Command #01.</p> |
| parameters | CommandEvent.ParametersEntry | repeated | <p>A generic map of parameters (name, value). The parameters are defined in the Diatheke model. Depending on the command, these parameters should be sent back with the CommandStatus update.</p> |
| command_state_id | string |  | <p>ID to keep track of the dialog state when the command is requested. This field is required in the CommandStatus message so that Diatheke can correctly update the dialog state when CommandFinished is called.</p> |







### Message: CommandEvent.ParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | string |  | <p></p> |
| value | string |  | <p></p> |







### Message: CommandStatus
The final status of an executed command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>session_id should be the same as the status id returned from NewSessionResponse.</p> |
| command_id | string |  | <p>ID of the command as given in the RunCommand object.</p> |
| return_status | CommandStatus.StatusCode |  | <p></p> |
| output_parameters | CommandStatus.OutputParametersEntry | repeated | <p>The populated output parameters from the RunCommand object. For example, the map might contain the entry "temperature", which was populated with a value of "30" after the command finished.</p> |
| error_message_text | string |  | <p>Set this field with an error message if an a error occured while executing the command.</p> |
| command_state_id | string |  | <p>State ID from the original CommandEvent. This field is required for Diatheke to correctly update the dialog state when CommandFinished is called.</p> |







### Message: CommandStatus.OutputParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | string |  | <p></p> |
| value | string |  | <p></p> |







### Message: DiathekeEvent
An event from Diatheke in response to either recognized audio or
submitted text.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| command | CommandEvent |  | <p>Indicates Diatheke found an actionable state in the dialog, and requests the client to perform the given command.</p><p>While not strictly required (depeding on the model and command), it is best practice to always call CommandFinished after receiving this event so that Diatheke can update the dialog state when the command is complete. Cases where it is required include when the command has output parameters, or when it is followed by another action in the Diatheke model.</p> |
| recognize | RecognizeEvent |  | <p>An event indicating whether pushed text and audio was recognized by ASR and/or Diatheke.</p> |
| reply | ReplyEvent |  | <p>The textual reply from Diatheke in the conversation (not to be confused with the server concepts of request and response). For example, this could be a question to solicit more information from the user, a status report, or any other reply defined by the Diatheke model. The text of this message is also provided in the AudioReply stream (if one is open).</p> |







### Message: Empty



This message is empty and has no fields.






### Message: ModelsResponse
The message sent by the server in response to a Models request.
Returns an array of model names.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| models | string | repeated | <p>Array of models available for use.</p> |







### Message: NewSessionRequest
Request for the NewSession call.



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | string |  | <p>For applications that have more than one model to use for ASR/NLU. ASR grammar can vary between models, as well as sets of commands. Some applications will only have one model.</p> |







### Message: PushTextRequest
Request to push text to Diatheke as part of a conversation.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |
| text | string |  | <p>User input. This could be a transcription from manually run ASR, text selected from a dropdown list, entered in a prompt, etc.</p> |







### Message: RecognizeEvent
A RecognizeEvent occurs if a session's audio input has a transcription
available, or if the PushText method was called. In both cases, the
event will indicate whether the text was recognized as a valid intent
by the Diatheke model.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | string |  | <p>The pushed text or transcription of audio sent to Diatheke.</p> |
| valid_input | bool |  | <p>True if the submitted text or audio transcription was recognized by the Diatheke model as a valid intent or entity.</p> |







### Message: ReplyEvent
A ReplyEvent occurs when Diatheke has a reply in the conversation (not
to be confused with the server concepts of request and response). These
correspond to "say" entries in the Diatheke model. For example, it might
be a prompt for additional information from the user, a status update,
or a confirmation. ReplyEvents are not generated in response to 
StreamTTS calls.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| text | string |  | <p>Text of the reply event (defined by the Diatheke model).</p> |
| label | string |  | <p>Label of the reply event (defined by the Diatheke model).</p> |







### Message: SessionID
Simple message that only contains the session ID.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |







### Message: TTSRequest
Request to synthesize speech unrelated to a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | string |  | <p>The model to use for TTS (defined in the server config file).</p> |
| text | string |  | <p>Text to synthesize</p> |







### Message: TTSResponse
Response for text-to-speech unrelated to a session.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| data | bytes |  | <p>The synthesized audio data. The data encoding will match what was specified in the server configuration.</p> |







### Message: VersionResponse
The message sent by the server for the `Version` method.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| server | string |  | <p>Server that manages all of the the other components.</p> |





 <!-- end messages -->



### Enum: CommandStatus.StatusCode
CommandStatus are the resulting states of a command.

| Name | Number | Description |
| ---- | ------ | ----------- |
| SUCCESS | 0 | SUCCESS indicates that the command was successfully completed, and the dialog state may now move on to the next state. |
| FAILURE | 1 | FAILURE indicates that the command was not successfully completed, and the dialog state should be updated accordingly. |


 <!-- end enums -->

 <!-- end HasExtensions -->




## Scalar Value Types

| .proto Type | Notes | Go Type | Python Type |
| ----------- | ----- | ------- | ----------- |
| double |  | float64 | float |
| float |  | float32 | float |
| int32 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint32 instead. | int32 | int |
| int64 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint64 instead. | int64 | int/long |
| uint32 | Uses variable-length encoding. | uint32 | int/long |
| uint64 | Uses variable-length encoding. | uint64 | int/long |
| sint32 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int32s. | int32 | int |
| sint64 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int64s. | int64 | int/long |
| fixed32 | Always four bytes. More efficient than uint32 if values are often greater than 2^28. | uint32 | int |
| fixed64 | Always eight bytes. More efficient than uint64 if values are often greater than 2^56. | uint64 | int/long |
| sfixed32 | Always four bytes. | int32 | int |
| sfixed64 | Always eight bytes. | int64 | int/long |
| bool |  | bool | boolean |
| string | A string must always contain UTF-8 encoded or 7-bit ASCII text. | string | str/unicode |
| bytes | May contain any arbitrary sequence of bytes. | []byte | str |


