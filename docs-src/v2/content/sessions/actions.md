---
title: "Session Actions"
description: "Defines the actions an application should handle on behalf of a session."
weight: 520
---

Session actions indicate what Diatheke expects the SDK app to do next, and
may be one of the following types:

* [WaitForUser](#wait-for-user-action)
* [Reply](#reply-action)
* [Command](#command-action)

## Processing the Action List
There are currently two ways an app will receive an action list - when
[creating a session](../create), and when [updating a session](../update).
These action lists will always end with an action that requires a session
update (such as a WaitForUserAction or a CommandAction), which will then
return a new action list. This cycle of handling the session actions then
updating the session repeats indefinitely, until the app decides it is
time to [delete the session](../delete).

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
            # The CommandAction will involve a session update
            return handle_command(client, session, action.command)
        else:
            raise RuntimeError("unknown action={}".format(action))
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}

## Wait For User Action
The WaitForUserAction indicates that the SDK app should wait for input
from the user before continuing the dialog. This action requires a session
update, which depending on whether the input is audio data (speech) or
text, will come as a call to [Process Text](../update/#process-text) or
[Process ASR Result](../update/#process-asr-result).

Here is an example showing an app waiting for text input from the user.

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
// Example coming soon!
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}


Here is an example showing the app waiting for speech input from the user
(see also [Audio Input](../../audio-input)).

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
	// The given input action has a couple of flags to help the app
	// decide when to begin recording audio.
	if inputAction.Immediate {
		// This action is likely waiting for user input in response to
		// a question Diatheke asked, in which case the user should
		// reply immediately. If this flag is false, the app may wait
		// as long as it wants before processing user input (such as
		// waiting for a wake-word below).
	}

	if inputAction.RequiresWakeWord {
		// This action requires the wake-word to be spoken before
		// any other audio will be processed. Use a wake-word detector
		// and wait for it to trigger.
	}

	var result *diathekepb.ASRResult
	// The app should add code here to record audio and get a
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
    # app decide when to begin recording audio.
    if input_action.immediate:
        # This action is likely waiting for user input in response
        # to a question Diatheke asked, in which case the user should
        # reply immediately. If this flag is false, the app may wait
        # as long as it wants before processing user input (such as
        # waiting for a wake-word below).
        pass
    
    if input_action.requires_wake_word:
        # This action requires the wake-word to be spoken before
        # any other audio will be processed. Use a wake-word detector
        # and wait for it to trigger.
        pass

    result = None
    # The app should add code here to record audio and get a
    # result from an ASR stream.

    # Update the session with the result.
    print("  ASRResult: {}".format(result))
    return client.process_asr_result(session.token, result)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}

## Reply Action
The ReplyAction indicates that the SDK app should give the information
provided in the reply's text to the user. For apps using audio, see also
[Audio Output](../../audio-output).

{{< tabs >}}

{{< tab "Go" "go" >}}
// handleReply prints the given reply text to stdout.
func handleReply(client *diatheke.Client, reply *diathekepb.ReplyAction) {
	fmt.Printf("  Reply: %v\n", reply.Text)

	// If the app is using audio, it should use the given reply to
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
    
    # If the app is using audio, it should use the given reply to
    # create a TTS stream
    stream = client.new_tts_stream(reply)

    # Add code here to read audio data from the stream and send it to the
    # playback device.
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}

## Command Action
The CommandAction indicates the SDK app should execute a specified task.
This action requires a session update when the task is complete (see
[Process Command Result](../update/#process-command-result)).

The specific details of the command (purpose, ID, input parameters, etc.)
are captured in the Diatheke model, and can be anything that the SDK app
wants to execute. For example, it might be a task to lookup information in
a database and return it to Diatheke or to make a light blink on a device.
The specific task is indicated by the command ID.

For long-running tasks, it is recommended to use the command to launch the
task in the background and return a CommandResult to Diatheke quickly so
that the user may continue to do other things with the dialog in the
meantime. When the long-running task is finished, the app may optionally
use the [Set Story method](../update/#set-story) to interrupt the dialog
and give the results of the task to the user.

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

    # Update the session with the command result
    result = diatheke.CommandResult(id=cmd.id)
    return client.process_command_result(session.token, result)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Example coming soon!
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}
