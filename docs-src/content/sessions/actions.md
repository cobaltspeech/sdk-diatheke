---
title: "Session Actions"
description: "Defines the actions an application should handle on behalf of a session."
weight: 520
---

Session actions indicate what the Diatheke model requires the calling application
to do next, and may be one of the following types:

* [WaitForUser](#wait-for-user-action)
* [Reply](#reply-action)
* [Command](#command-action)
* [Transcribe](#transcribe-action)

## Processing the Action List
There are currently two ways an application will receive an action list - when
[creating a session](../create/), and when [updating a session](../update/).
These action lists will always end with an action that requires a session
update (such as a WaitForUserAction or a CommandAction), which will then
return a new action list. This cycle of handling the session actions then
updating the session repeats indefinitely, until the application decides it is
time to [delete the session](../delete/).

Here is an example showing how to process a typical action list:

{{< tabs >}}

{{< tab "Go" "go" >}}
// processActions executes the actions for the given session
// and returns an updated session.
func processActions(client *diatheke.Client, session *diathekepb.SessionOutput,
) (*diathekepb.SessionOutput, error) {
	// Iterate through each action in the list and determine its type.
	for _, action := range session.ActionList {
		if inputAction := action.GetInput(); inputAction != nil {
			// The WaitForUserAction will involve a session update.
			return waitForInput(client, session, inputAction)
		} else if reply := action.GetReply(); reply != nil {
			// Replies do not require a session update.
			handleReply(client, reply)
		} else if cmd := action.GetCommand(); cmd != nil {
			// The CommandAction will involve a session update.
			return handleCommand(client, session, cmd)
		} else if scribe := action.GetTranscribe(); scribe != nil {
			// Transcribe actions do not require a session update.
			err := handleTranscribe(client, scribe)
			if err != nil {
				return nil, err
			}
		} else if action.Action != nil {
			return nil, fmt.Errorf("received unknown action type %T", action.Action)
		}
	}

	return nil, fmt.Errorf("action list ended without session update")
}
{{< /tab >}}

{{< tab "Python" "python" >}}
def process_actions(client, session):
    """Executes the actions for the given session and returns
    an updated session."""

    # Iterate through each action in the list and determine its type
    for action in session.action_list:
        if action.HasField("input"):
            # The WaitForuserAction will involve a session update.
            return wait_for_input(client, session, action.input)
        elif action.HasField("reply"):
            # Replies do not require a session update.
            handle_reply(client, action.reply)
        elif action.HasField("command"):
            # The CommandAction will involve a session update.
            return handle_command(client, session, action.command)
        elif action.HasField("transcribe"):
            # Transcribe actions do not require a session update.
            handle_transcribe(client, action.transcribe)
        else:
            raise RuntimeError("unknown action={}".format(action))
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Executes the actions for the given session and returns
// an updated session.
cobaltspeech::diatheke::SessionOutput processActions(
    Diatheke::Client *client,
    const cobaltspeech::diatheke::SessionOutput &session) {
    // Iterate through each action in the list and determine its type.
    for (auto action : session.action_list()) {
        if (action.has_input()) {
            // The WaitForUserAction will involve a session update.
            return waitForInput(client, session, action.input());
        } else if (action.has_reply()) {
            // Replies do not require a session update.
            handleReply(action.reply());
        } else if (action.has_command()) {
            // The CommandAction will involve a session update.
            return handleCommand(client, session, action.command());
        } else if (action.has_transcribe()) {
            // Transcribe actions do not require a session update.
            handleTranscribe(client, action.transcribe());
        } else {
            throw std::runtime_error("received unknown action type");
        }
    }

    throw std::runtime_error("action list ended without session update");
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// processActions executes the actions for the given session.
func processActions(sessionOutput: Cobaltspeech_Diatheke_SessionOutput) {
	self.token = sessionOutput.token
	for actionData in sessionOutput.actionList {
		guard let action = actionData.action else { continue }
		switch action {
		case .input(let waitForUserAction):
			// The WaitForuserAction will involve a session update.
			self.waitForInput(waitForUserAction)
		case .reply(let replyAction):
			// Replies do not require a session update.
			self.handleReply(replyAction)
		case .command(let commandAction):
			// The CommandAction will involve a session update
			self.handleCommand(commandAction)
		}
	}
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void executeActions(SessionOutput resp) {
    List<ActionData> actionsToExecute = resp.getActionListList();

    for (ActionData action : actions) {
        switch (action.getActionCase()) {
            case INPUT:
                WaitForUserAction input = action.getInput();
                break;

            case COMMAND:
                CommandAction cmd = action.getCommand();
                handleCommandRequest(cmd);
                break;

            case REPLY:
                ReplyAction reply = action.getReply();
                handleReplyAction(reply);
                break;

            case ACTION_NOT_SET:
                Log.i(TAG, "ACTION_NOT_SET Response :/");
                break;
        }
    }
}
{{< /tab >}}

{{< /tabs >}}

## Wait For User Action
The WaitForUserAction indicates that the calling application should wait for input
from the user before continuing the dialog. This action requires a session
update, which depending on whether the input is audio data (speech) or
text, will come as a call to [Process Text](../update/#process-text) or
[Process ASR Result](../update/#process-asr-result).

Here is an example showing an application waiting for text input from the user.

{{< tabs >}}

{{< tab "Go" "go" >}}
// waitForInput prompts the user for text input, then updates the
// session based on the user-supplied text.
func waitForInput(
	client *diatheke.Client,
	session *diathekepb.SessionOutput,
	inputAction *diathekepb.WaitForUserAction,
) (*diathekepb.SessionOutput, error) {
	// Display a prompt
	fmt.Printf("\n\nDiatheke> ")

	// Wait for user input on stdin
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	text := scanner.Text()

	// Update the session with the text
	return client.ProcessText(context.Background(), session.Token, text)
}
{{< /tab >}}

{{< tab "Python" "python" >}}
def wait_for_input(client, session, input_action):
    """Prompts the user for text input, then returns an updated
    session based on the user-supplied text."""

    # Wait for user input
    text = input("\n\nDiatheke> ")

    # Update the session with the text
    return client.process_text(session.token, text)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Prompts the user for text input, then returns an updated
// session based on the user-supplied text.
cobaltspeech::diatheke::SessionOutput waitForInput(
    Diatheke::Client *client,
    const cobaltspeech::diatheke::SessionOutput &session,
    const cobaltspeech::diatheke::WaitForUserAction &inputAction) {
    std::cout << std::endl << "\nDiatheke> " << std::flush;

    // Wait for user input
    std::string text;
    std::getline(std::cin, text);

    // Update the session with the text.
    return client->processText(session.token(), text);
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func waitForInput(_ action: Cobaltspeech_Diatheke_WaitForUserAction) {
	// In a real application `waitForInput` function may be used as the result handler
	// for `UITextField`'s text change or for other user input ways.
	let text = "some text input"
	// Update the session with the text
	client.processText(token: token, text: text) { (sessionOutput) in
		self.processActions(sessionOutput: sessionOutput)
	} failure: { (error) in
 		print(error.localizedDescription)
	}
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
public List<ActionData> waitForInput() throws StatusRuntimeException {
    // In a real application `waitForInput` function may be called from a button or editText listener.
    String text = "some text input"
    TextInput txtMsg = TextInput.newBuilder()
            .setText(text)
            .build();
    SessionInput input = SessionInput.newBuilder()
            .setToken(mToken)
            .setText(txtMsg)
            .build();
    SessionOutput resp = mDiathekeBlockingService.updateSession(input);
    executeActions(resp);
}
{{< /tab >}}

{{< /tabs >}}


Here is an example showing the application waiting for speech input from the user
(see also [Audio Input](../../audio-input/)).

{{< tabs >}}

{{< tab "Go" "go" >}}
// waitForInput creates an ASR stream and records audio from the user.
// The audio is sent to Diatheke until an ASR result is returned, which
// is used to return an updated session.
func waitForInput(
	client *diatheke.Client,
	session *diathekepb.SessionOutput,
	inputAction *diathekepb.WaitForUserAction,
) (*diathekepb.SessionOutput, error) {
	// The given input action has a couple of flags to help the application
	// decide when to begin recording audio.
	if inputAction.Immediate {
		// This action is likely waiting for user input in response to
		// a question Diatheke asked, in which case the user's audio should
		// be streamed back to Diatheke immediately. If this flag is false,
		// the application may wait as long as it wants before processing
		// user input (such as waiting for a wake-word below).
	}

	if inputAction.RequiresWakeWord {
		// This action requires the wake-word to be spoken before
		// any other audio will be processed. Use a wake-word detector
		// and wait for it to trigger.
	}

	var result *diathekepb.ASRResult
	// The application should add code here to record audio and get a
	// result from an ASRStream.

	// Update the session with the result
	fmt.Printf("  ASRResult: %v\n\n", result)
	return client.ProcessASRResult(context.Background(), session.Token, result)
}
{{< /tab >}}

{{< tab "Python" "python" >}}
def wait_for_input(client, session, input_action):
    """Creates a new ASR stream and records audio from the user.
    The audio is sent to Diatheke until an ASR result is returned,
    which is used to return an updated session."""

    # The given input action has a couple of flags to help the
    # application decide when to begin recording audio.
    if input_action.immediate:
        # This action is likely waiting for user input in response to
        # a question Diatheke asked, in which case the user's audio should
        # be streamed back to Diatheke immediately. If this flag is false,
        # the application may wait as long as it wants before processing
        # user input (such as waiting for a wake-word below).
        pass
    
    if input_action.requires_wake_word:
        # This action requires the wake-word to be spoken before
        # any other audio will be processed. Use a wake-word detector
        # and wait for it to trigger.
        pass

    result = None
    # The application should add code here to record audio and get a
    # result from an ASR stream.

    # Update the session with the result.
    print("  ASRResult: {}".format(result))
    return client.process_asr_result(session.token, result)
{{< /tab >}}

{{< tab "C++" "c++" >}}
/*
 * Prompts the user for text input, then returns an updated
 * session based on the user-supplied text.
 */
cobaltspeech::diatheke::SessionOutput waitForInput(
    Diatheke::Client *client,
    const cobaltspeech::diatheke::SessionOutput &session,
    const cobaltspeech::diatheke::WaitForUserAction &inputAction) {
    /*
     * The given input action has a couple of flags to help
     * the app decide when to begin recording audio.
     */
    if (inputAction.immediate()) {
        /*
         * This action is likely waiting for user input in response
         * to a question Diatheke asked, in which case the user's audio
         * should be streamed back to Diatheke immediately. If this flag
         * is false, the app may wait as long as it wants before processing
         * user input (such as waiting for a wake-word below).
         */
    }

    if (inputAction.requires_wake_word()) {
        /*
         * This action requires the wake-word to be spoken before
         * the user input will be accepted. Use a wake-word detector
         * and wait for it to trigger.
         */
    }

    cobaltspeech::diatheke::ASRResult result;
    /*
     * The application should add code here to record audio and get a
     * result from an ASR stream.
     */
  
    // Update the session with the result
    std::cout << "\n  ASRResult:" << std::endl;
    std::cout << "    Text: " << result.text() << std::endl;
    std::cout << "    Confidence: " << result.confidence() << std::endl;

    return client->processASRResult(session.token(), result);
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func waitForInput(_ action: Cobaltspeech_Diatheke_WaitForUserAction) {
	/* Creates a new ASR stream and records audio from the user.
	   The audio is sent to Diatheke until an ASR result is returned,
	   which is used to return an updated session. */
	
	// The given input action has a couple of flags to help the app
	// decide when to begin recording audio.
	if action.immediate {
		// This action is likely waiting for user input in response to
		// a question Diatheke asked, in which case the user's audio should
		// be streamed back to Diatheke immediately. If this flag is false,
		// the application may wait as long as it wants before processing
		// user input (such as waiting for a wake-word below).
	}

	if action.requiresWakeWord {
		// This action requires the wake-word to be spoken before
		// any other audio will be processed. Use a wake-word detector
		// and wait for it to trigger.
	}

	var asrResult = Cobaltspeech_Diatheke_ASRResult()
	// The app should add code here to record audio and get a
	// result from an ASRStream.
	print("ASRResult: \(asrResult)")

	// Update the session with the result
	client.processASRResult(token: token, asrResult: asrResult) { (sessionOutput) in
		self.processActions(sessionOutput: sessionOutput)
	} failure: { (error) in
		print(error.localizedDescription)
	}
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void waitForInput(_ action: Cobaltspeech_Diatheke_WaitForUserAction) {
	/* Creates a new ASR stream and records audio from the user.
	   The audio is sent to Diatheke until an ASR result is returned,
	   which is used to return an updated session. */
	
	// The given input action has a couple of flags to help the app
	// decide when to begin recording audio.
	if (action.immediate) {
		// This action is likely waiting for user input in response to
		// a question Diatheke asked, in which case the user's audio should
		// be streamed back to Diatheke immediately. If this flag is false,
		// the application may wait as long as it wants before processing
		// user input (such as waiting for a wake-word below).
	}

	if (action.requiresWakeWord) {
		// This action requires the wake-word to be spoken before
		// any other audio will be processed. Use a wake-word detector
		// and wait for it to trigger.
	}

	ASRResult asrResult = new ASRResult();
	var asrResult = Cobaltspeech_Diatheke_ASRResult()
	// The app should add code here to record audio and get a
	// result from an ASRStream.
	Log.v(TAG, "ASRResult: " + asrResult);

	// Update the session with the result
	SessionInput input = SessionInput.newBuilder()
		.setToken(mToken)
		.setAsr(asrResult)
		.build();
	SessionOutput resp = mDiathekeBlockingService.updateSession(input);
}
{{< /tab >}}

{{< /tabs >}}

There are two flags in the WaitForUser action, `Immediate` and `RequiresWakeWord`.
An Immediate WaitForUserAction is appropriate for input required to fulfill or 
continue a specific interaction and is usually in response to a prompt from Diatheke.
For example, if the user's previous input was ambiguous or missing a required 
parameter, Diatheke will prompt for clarification:
```
> User: I want to reserve a table for 2 on Friday.
> Diatheke: What time do you want the reservation?
> User: 7 p.m.
```

If the action does not have the Immediate flag set, the specific interaction is
complete and the system is waiting for whatever the user may next request, similar
to an idle state.

If the application is using a separate wake-word detector, the `RequiresWakeWord`
flag will indicate when audio should be sent to the wake word detector. Once the
wake word has been triggered, audio should again be sent to Diatheke.

## Reply Action
The ReplyAction indicates that the calling application should give the information
provided in the reply's text to the user. For applications using audio, see also
[Audio Output](../../audio-output/).

{{< tabs >}}

{{< tab "Go" "go" >}}
// handleReply prints the given reply text to stdout.
func handleReply(client *diatheke.Client, reply *diathekepb.ReplyAction) {
	fmt.Printf("  Reply: %v\n", reply.Text)

	// If the application is using audio, it should use the given reply to
	// create a TTS stream.
	stream, err := client.NewTTSStream(context.Background(), reply)

	// Add code here to read audio data from the stream and send it to the
	// playback device.
}
{{< /tab >}}

{{< tab "Python" "python" >}}
def handle_reply(client, reply):
    """Prints the given reply to stdout."""

    print("  Reply:", reply.text)
    
    # If the application is using audio, it should use the given reply to
    # create a TTS stream
    stream = client.new_tts_stream(reply)

    # Add code here to read audio data from the stream and send it to the
    # playback device.
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Prints the text of the given reply to stdout.
void handleReply(const cobaltspeech::diatheke::ReplyAction &reply) {
    std::cout << "\n  Reply:" << reply.text() << std::endl;

    /*
     * If the application is using audio, it should use the given reply to
     * create a TTS stream.
     */
    Diatheke::TTSStream stream = client->newTTSStream(reply);

    /* Add code here to read audio data from the stream and send it to the
     * playback device.
     */
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func handleReply(_ replyAction: Cobaltspeech_Diatheke_ReplyAction) {
	// If the app is using audio, it should use the given reply to
	// create a TTS stream
	self.ttsStream = self.client.newTTSStream(replyAction: replyAction, dataChunkHandler: { (ttsAudio) in
		// Add code here to read audio data from the stream and send it to the
		// playback device.
	}, completion: { (error) in
		// Add code here to handle TTS errors and TTS streaming completion
		if let error = error {
			print("TTS error received: \(error)")
		}
	})
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void handleReplyAction(ReplyAction reply) {
	// Request a call to TTS.
	try {
		Iterator<TTSAudio> ttsResp = mDiathekeBlockingService.streamTTS(reply);

		// Collect the audio into a single array
		ByteArrayOutputStream bb = new ByteArrayOutputStream();
		while (ttsResp.hasNext()) {
			bb.write(ttsResp.next().getAudio().toByteArray());
		}

        // Add code here to handle the full audio, add a wave header, save to a temp file, play via MediaPlayer, etc.

	} catch (Exceptoin e) {
        // Handle errors here.
    }
}


{{< /tab >}}

{{< /tabs >}}

## Command Action
The CommandAction indicates the calling application should execute a specified task.
This action requires a session update to indicate when the task is complete (see
[Process Command Result](../update/#process-command-result)).

The specific details of the command (purpose, ID, input parameters, etc.)
are defined in the Diatheke model, and can be anything that the calling application
wants to execute. The command ID is used to indicate which specific task Diatheke
wants the application to run. Example command actions might include:

* Look up information in a database and return it to Diatheke, such as the weather.
* Make a light blink on a device, such as a notification LED.
* Updating the current display in a UI with additional information.

{{% panel theme="info" header="Note" %}}
For long-running tasks, it is recommended to use the command to launch the
task in the background and return a CommandResult to Diatheke quickly so
that the user may continue to do other things with the dialog in the
meantime. When the long-running task is finished, the application may optionally
use the <a href="../update/#set-story">Set Story method</a> to interrupt the dialog
and give the results of the task to the user.
{{% /panel %}}

{{< tabs >}}

{{< tab "Go" "go" >}}
// handleCommand executes the specified command.
func handleCommand(
	client *diatheke.Client,
	session *diathekepb.SessionOutput,
	cmd *diathekepb.CommandAction,
) (*diathekepb.SessionOutput, error) {
	fmt.Printf("  Command:\n")

	// The command ID indicates which task to execute
	fmt.Printf("    ID: %v\n", cmd.Id)

	// The input parameters are extracted from prior session input and
	// give information relevant to the command.
	fmt.Printf("    Input params: %v\n\n", cmd.InputParameters)

	// Add application-specific code to execute the command.

	// Update the session with the command result
	result := diathekepb.CommandResult{Id: cmd.Id}
	return client.ProcessCommandResult(context.Background(), session.Token, &result)
}
{{< /tab >}}

{{< tab "Python" "python" >}}
def handle_command(client, session, cmd):
    """Executes the task specified by the given command and
    returns an updated session based on the command result."""

    print("  Command:")

    # The command ID indicates which task to execute
    print("    ID:", cmd.id)

    # The input parameters are extracted from prior session input and
    # give information relevant to the command.
    print("    Input params:", cmd.input_parameters)

	# Add application-specific code to execute the command.

    # Update the session with the command result
    result = diatheke.CommandResult(id=cmd.id)
    return client.process_command_result(session.token, result)
{{< /tab >}}

{{< tab "C++" "c++" >}}
/*
 * Executes the task specified by the given command and
 * returns an updated session based on the command result.
 */
cobaltspeech::diatheke::SessionOutput handleCommand(
    Diatheke::Client *client,
    const cobaltspeech::diatheke::SessionOutput &session,
    const cobaltspeech::diatheke::CommandAction &cmd) {
    std::cout << "\n  Command:" << std::endl;

    // The command ID indicates which task to execute
    std::cout << "    ID:" << cmd.id() << std::endl;
    std::cout << "    Input params:" << std::endl;

    /*
     * The input parameters are extracted from prior session input and
     * give information relevant to the command. In C++ the data structure
     * is similar to a std::map.
     */
    auto params = cmd.input_parameters();
    for (auto iter = params.begin(); iter != params.end(); iter++) {
      std::cout << "      " << iter->first << " = " << iter->second << std::endl;
    }

    // Add application-specific code to execute the command.

    // Update the session with the command result
    cobaltspeech::diatheke::CommandResult result;
    result.set_id(cmd.id());
    return client->processCommandResult(session.token(), result);
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Executes the task specified by the given command
func handleCommand(_ commandAction: Cobaltspeech_Diatheke_CommandAction) {
	print("Command:")
	// The command ID indicates which task to execute
	print("  ID: \(commandAction.id)")
	// The input parameters are extracted from prior session input and
	// give information relevant to the command.
	print("  Params: \(commandAction.inputParameters)")

	// Update the session with the command result
	var commandResult = Cobaltspeech_Diatheke_CommandResult()
	commandResult.id = commandAction.id
	client.processCommandResult(token: token, commandResult: commandResult) { (sessionOutput) in
		self.processActions(sessionOutput: sessionOutput)
	} failure: { (error) in
		print(error.localizedDescription)
	}
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void handleCommandRequest(CommandAction cmd) {
    // Log the incoming command and parameters.
    Log.i(TAG, String.format(Locale.US, "Command ID: %s", cmd.getId()));
    for (Map.Entry<String, String> entry : cmd.getInputParametersMap().entrySet()) {
        Log.i(TAG, String.format(Locale.US, "{%s: %s}", entry.getKey(), entry.getValue()));
    }

    // Each command should have a result sent.  We will just do a hardcoded that response.
    CommandResult cmdResult = CommandResult.newBuilder()
            .putOutParameters("Param1", "Value1")
            .putOutParameters("Param2", "Value2")
            .setId(cmd.getId())
            .setError("")
            .build();

    Log.i(TAG, "Sending command results now");        SessionInput input = SessionInput.newBuilder()
            .setToken(mToken)
            .setCmd(commandResult)
            .build();
    SessionOutput resp = mDiathekeBlockingService.updateSession(input);
    executeActions(resp.getActionListList());
}
{{< /tab >}}

{{< /tabs >}}

## Transcribe Action
The TranscribeAction indicates the calling application should use ASR
to get a transcription from the user. Unlike the [WaitForUser](#wait-for-user-action),
this action does not have any effect on the Diatheke session, because
the purpose of the requested transcript is application specific and
is unrelated to the ongoing conversation with Diatheke. For example,
an application may have a TranscribeAction that is used to record a
note which is then saved in a database for later recall. In this case
the transcription is not used to update the session, but is simply
saved. After the user is done taking a note, session processing
continues to handle replies, commands, etc.

When an application receives a TranscribeAction, it should use the
[Transcribe method](../../audio-input/#transcribe) for ASR processing, which has slightly different
behaviors than the [StreamASR method](../../audio-input/#streamasr).

{{< tabs >}}

{{< tab "Go" "go" >}}
// handleTranscribe uses ASR to record a transcription from the user.
func handleTranscribe(client *diatheke.Client, scribe *diathekepb.TranscribeAction) error {
	fmt.Printf("TranscribeAction:\n")

	// The ID helps the application identify the purpose of this
	// transcription (e.g., taking a note).
	fmt.Printf("  ID: %v\n", scribe.Id)

	// The Cubic model ID specifies which ASR model to use for
	// transcription (defined by the server).
	fmt.Printf("  Cubic Model ID: %v\n", scribe.CubicModelId)

	// The Diatheke model ID specifies which Diatheke model to use
	// to check for stream-end conditions (e.g., timeout).
	fmt.Printf("  Diatheke Model ID: %v\n", scribe.DiathekeModelId)

	// The application should add code here to record audio and process
	// results using the TranscribeStream, which is created using the
	// given transcribe action.

	return nil
}
{{< /tab >}}

{{< tab "Python" "python" >}}
def handle_transcribe(client, scribe):
    """Uses ASR to record a transcription from the user."""

    print("TranscribeAction:")

    # The ID helps the application identify the purpose of this
    # transcription (e.g., taking a note).
    print("  ID: " scribe.id)

    # The Cubic model ID specifies which ASR model to use for
    # transcription (defined by the server).
    print("  Cubic Model ID: ", scribe.cubic_model_id)

    # The Diatheke model ID specifies which Diatheke model to use
    # to check for stream-end conditions (e.g., timeout).
    print("  Diatheke Model ID: ", scribe.diatheke_model_id)

    # The application should add code here to record audio and process
    # results from the TranscribeStream, which is created using the
    # given transcribe action.
{{< /tab >}}

{{< tab "C++" "c++" >}}
/*
 * Records user audio for the purpose of transcription.
 */
void handleTranscribe(Diatheke::Client *client,
                      const DiathekePB::TranscribeAction &scribe) {
    std::cout << "TranscribeAction" << std::endl;

    /*
     * The ID helps the application identify the purpose of this
     * transcription (e.g., taking a note).
     */
    std::cout << "  ID: " << scribe.id() << std::endl;

    /*
     * The Cubic model ID specifies which ASR model to use for
     * transcription (defined by the server).
     */
    std::cout << "  Cubic Model ID: ", scribe.cubic_model_id() << std::endl;

    /*
     * The Diatheke model ID specifies which Diatheke model to use
     * to check for stream-end conditions (e.g., timeout).
     */
    std::cout << "  Diatheke Model ID: ", scribe.diatheke_model_id() << std::endl;

    /*
     * The application should add code here to record audio and process
     * results from the TranscribeStream, which is created using the
     * given transcribe action.
     */
}
{{< /tab >}}

{{< /tabs >}}
