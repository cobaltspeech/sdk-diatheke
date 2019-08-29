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
| Models | Empty | ModelsResponse | Models will return a list of available versions. This value should be used to the NewSession calls. |
| NewSession | NewSessionRequest | NewSessionResponse | Requests a new session with the given config, returns a new ID. This new ID should be used with all future calls. |
| EndSession | SessionEndRequest | Empty | Terminates an existing session. It is an error if the SessionEndRequest has an invalid SessionID. |
| PushAudio | AudioTranscriptionRequest | TranscriptionResult | Performs bidirectional streaming speech recognition. As transcripts are generated, the result is both sent to the caller and passed on to the dialog management system. The dialog management path happens asyncronously, and will update the application manager through the CommandCallback and SayCallback calls.

TODO, since this is a streaming api, and we endpoint once it closes, it is hard to support an HTTP interface. The sim server would need to keep this call alive between calls to their PushAudio API, and maybe have their own EndPoint API that is responsible for shutting down the old one. Also, they would need to keep a call for each session. Would it make sense to have a streaming call remain open, one for each session in a map. If there is an existing connection, then push the next audio clip on the existing one. If there is not an exist connection, then we create a new connection which we then store and use. When their users call the EndpointAudio API, they can close the existing connection, and remove it from the map. Is that better or worse than adding an EndpointAudio gRPC call? Since it seems doable without the EndpointAudio gRPC call, I think the last question to ask is how do we handle responses. Returning a stream of TranscriptionResults means they have to start/stop the callback listener, but that doesn't seem to be a problem. The storage on their side is the same either way. Jacobs thoughts: I think it should be ok to exclude the EndpointAudio gRPC, endpoint when the PushAudio

TODO, can a single session have mroe than one audio stream? While it seems like it could be valid, I think it's fine to lock it down to one stream per session. |
| PushText | PushTextRequest | Empty | PushText will sidestep the ASR portion and simply push a "Transcript" directly to the dialog management. The dialog management path will happen asyncronously via the CommandCallback and SayCallback calls, same as with PushAudio.

For frontends that display a transcript of the conversation, transcribed text will be streamed back from the PushAudio call. Text sent via the PushText will not be returned, so you may add it to the history right away. |
| CommandAndNotify | CommandStatusUpdate | CommandToExecute | CommandAndNotify is a bi-directional stream that handles the interactions of commands and notifications between the client and server. By using the bi-directional streaming, either side can initiate a new chain of interactions.

Commands: When the dialog management finds enough context to indicate a command should be executed, it sends a Command object back to the application manager to execute.

Please note: When a new command that is accompanied by a TTS step is triggered, that TTS will be sent on the SayCallback rpc.

Notificiations: Notifications are sent by the application manager to update the dialog state. Depending on the application, there can be some delay between the initation of a command execution and the completion of that step. Some other applications may have multiple updates for the same command. Both of these benefit from making this an asynchronous pattern. |
| Say | TTSRequest | Empty | Say may be used to request TTS synthesis on given text, independent of any dialog state changes. Results will come back via the SayCallback stream. |
| SayCallback | SayCallbackRequest | TTSResponse | SayCallback is triggered whenever there is a new TTS audio clip that should be played for the user. This can come from the dialog management pushing a Say action. TTSResponses from the client making a Say(TTSRequest) call will be returned from that call, not through this callback.

Please note: When a new TTS that is accompanied by a Command is triggered, that Command will be sent on the CommandCallback rpc. |

 <!-- end services -->



### Message: AudioTranscriptionRequest
To use ASR, audio is streamed to the server.  Multiple
StreamingRecognizeRequest messages are sent. The first message must contain a
session_id message only.  All subsequent messages must contain audio data
only.  All AudioTranscriptionRequest messages must contain non-empty audio.
If audio content is empty, the server may interpret it as end of stream and
stop accepting any further messages.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |
| data | bytes |  | <p>Audio data that should be transcribed.</p> |







### Message: CommandStatusUpdate



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>session_id should be the same as the status id returned from NewSessionResponse.</p> |
| data | CommandStatusUpdate.Data |  | <p></p> |







### Message: CommandStatusUpdate.Data



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| command_id | string |  | <p>ID of the command that should be run. i.e. "COM01" for Command #01.</p> |
| command_status | CommandStatusUpdate.Data.CommandStatus |  | <p></p> |
| state_data | string |  | <p>The state_data field contains information about the state in which this notification should be handled. TODO: Why do we have this in the HTTP interface? Can we hide it in the gRPC interface?</p> |
| parameters | CommandStatusUpdate.Data.ParametersEntry | repeated | <p>parameters is a generic map of parameter name to parameter value. These parameters are defined in the Diatheke Models. An example could be the "TV Channel" is "Channel 2".</p> |
| error_message_text | string |  | <p>error_message_text will be populated when there is a error occures while executing a command. This string will be sent through TTS and posted to the SayCallback as an audio file.</p> |







### Message: CommandStatusUpdate.Data.ParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | string |  | <p></p> |
| value | string |  | <p></p> |







### Message: CommandToExecute
CommandToExecute indicates Diatheke found an actionable state in the dialog,
and requests the application manager to perform the command.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>session_id should be the same as the status id returned from NewSessionResponse.</p> |
| command_id | string |  | <p>ID of the command that should be run. i.e. "COM01" for Command #01.</p> |
| parameters | CommandToExecute.ParametersEntry | repeated | <p>parameters is a generic map of parameter name to parameter value. These parameters are defined in the Diatheke Models.</p> |
| state_data | string |  | <p>The state_data field contains information about the state in which this notification should be handled. Since there can be multiple states being tracked in a single session, this value must be sent with the CommandStatusUpdate that follow from the commands execution.</p> |







### Message: CommandToExecute.ParametersEntry



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| key | string |  | <p></p> |
| value | string |  | <p></p> |







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
TODO Concider combining the language and model into one string, such as
`en_US_variant`.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| model | string |  | <p>For applications that have more than one model to use for ASR/NLU. ASR grammer can vary between models, as well as sets of commands. Some applications will only have one model.</p> |







### Message: NewSessionResponse
Result of the NewSession call.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID that should be included with future calls. Allows for reconnecting if the gRPC connection is dropped. If an error occured, then this field will be an empty string ("").</p> |







### Message: PushTextRequest
Request for adding User Input directly from text.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |
| text | string |  | <p>User input, like a transcription from ASR.</p> |







### Message: SayCallbackRequest



| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |







### Message: SessionEndRequest
Request for the EndSession call.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |







### Message: TTSRequest
Request to stynthesize audio for a given string.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |
| text | string |  | <p>Text to synthesize.</p> |







### Message: TTSResponse
TTSResponse contains the sythesised audio waveform for a TTS request.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p></p> |
| text | string |  | <p>Text to synthesize. When responding to a Say(TTSRequest) call, it is the same string that was in TTSRequest.text. When it comes from a Notify(CommandStatusUpdate) call, it is the same string that was in the CommandStatusUpdate.error_message_text.</p> |
| data | bytes |  | <p>data contains the audio waveform as an array of bytes.</p> |







### Message: TranscriptionResult
TranscriptionResult will either be the transcribed text, or an error message.
TranscriptionResult is sent whenenver the Cubic engine endpoints an audio
segment.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| session_id | string |  | <p>Session ID returned from the NewSession call.</p> |
| text | string |  | <p>Text is the Cubic engine's formatted transcript of pushed audio. Transcribed text, with formatting applied. This field will be the 1-best alternative.</p> |
| confidence_score | double |  | <p>The confidence score is a floating point number 0.0-1.0, on how confident the ASR engine is in that transcription.</p> |







### Message: VersionResponse
The message sent by the server for the `Version` method.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| server | string |  | <p>Server that manages all of the the other components.</p> |





 <!-- end messages -->



### Enum: CommandStatusUpdate.Data.CommandStatus
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


