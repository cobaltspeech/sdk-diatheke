---
title: "Updating a Session"
description: "How to update the state of a session using one of the allowed inputs, then responding to the session output."
weight: 530
---

In the underlying [gRPC API](../../protobuf), the `UpdateSession` method
is called using one of several data types to update the session. For the
sake of convenience, the SDK provides several functions to handle each
type of session update. Updating the session always requires the current
session token and will return an updated token with a new [action list](../actions).

## Process ASR Result
When an [ASR stream](../../audio-input) returns a result, it may be used
directly to update the session. This is usually called in response to a
[Wait For User Action](../actions/#wait-for-user-action).

{{< tabs >}}

{{< tab "Go" "go" >}}
// Update the session with an ASR result returned previously.
session, err = client.ProcessASRResult(context.Background(), session.Token, result)
{{< /tab >}}

{{< tab "Python" "python" >}}
# Update the session with an ASR result returned previously.
session = client.process_asr_result(session.token, result)
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

## Process Text
Text supplied by a user may be used directly to update a session. This is
usually called in response to a [Wait For User Action](../actions/#wait-for-user-action).

{{< tabs >}}

{{< tab "Go" "go" >}}
// Update the session with text.
text := "what's the weather forecast for today"
session, err = client.ProcessText(context.Background(), session.Token, text)
{{< /tab >}}

{{< tab "Python" "python" >}}
# Update the session with text.
text = "what's the weather forecast for today"
session = client.process_text(session.token, text)
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

## Process Command Result
After receiving a [Command Action](../actions/#command-action), the SDK
app should always update the session with a command result.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Always use the original command ID in the result
result := diathekepb.CommandResult{Id: cmd.Id}

// If there are any output parameters expected by the model,
// add them to the result.
result.OutParameters = map[string]string{"key": "val"}

// If there was a fatal error, set the error message in the result.
result.Error = "I died"

// Update the session with the result
session, err = client.ProcessCommandResult(context.Background(), session.Token, &result)
{{< /tab >}}

{{< tab "Python" "python" >}}
# Always use the original command ID in the result
result = diatheke.CommandResult(id=cmd.id)

# If there are any output parameters expected by the model,
# add them to the result.
result.out_parameters["key"] = "val"

# If there was a fatal error, set the error message in the result.
result.error = "I died"

# Update the session with the result
session = client.process_command_result(session.token, result)
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

## Set Story
The `SetStory` method of the API may be used to update a session to run
from a specified "story" in the model. This method is particularly useful
for implementing system initiated alerts, or to update a session in a
mixed UI environment (GUI + voice).

Because this method completely overrides the current session state, it may
be called at any time, including in the middle of action list processing.

{{< tabs >}}

{{< tab "Go" "go" >}}
// The story ID is defined in the Diatheke model.
storyID := "alertStory"

// Set parameters in the updated session's memory (leave empty to
// clear memory).
params := map[string]string{"key": "val"}

// Update the session
session, err = client.SetStory(context.Background(), session.Token, storyID, params)
{{< /tab >}}

{{< tab "Python" "python" >}}
# The story ID is defined in the Diatheke model
story_id = "alertStory"

# Set parameters in the updates session's memory (leave empty to
# clear memory)
params = {"key": "val"}

# Update the session
session = client.set_story(session.token, story_id, params)
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
