---
title: "Sessions"
description: "Defines what a Diatheke session is, and how to interact with it."
weight: 500
---

## Overview
A session represents the state of a dialog between a user and Diatheke. It
tracks information relevant to the dialog, storing that information and
passing it to the SDK app as necessary. When the information is no longer
needed, it is removed from session memory. The Diatheke SDK encapsulates
the session state in a token that the SDK app must use when processing
user input and executing commands.

## SDK Methods
{{% children description="true" %}}

## Session Lifecycle
Every session is created using a Diatheke model, which defines the
expected paths a dialog may take, including what the user and the
application are allowed to do during the dialog. A session is typically
created when the SDK app wants to start a new dialog from the beginning,
with nothing saved in memory. At the time of creation, Diatheke will also
return a list of [actions](./actions) the app should take. While
processing these actions, the app will periodically need to update the
session, at which point Diatheke will return additional actions the app
should process. This cycle of processing actions and updating the session
continues until the app decides it is done with the session, at which
point it is deleted.

![Session Overview](/img/session_overview.png)

## Example Workflows
Below are a couple of examples showing how an app may interact with a
session to respond to user's request to get the weather forecast.

### Voice I/O
In this example, only voice interactions are allowed between the user and
the system (i.e., input is speech audio and output is synthesized speech
audio). As shown below, the app starts by [creating a session](./create),
which returns a session token and a
[WaitForUserAction](./actions/#wait-for-user-action). To get user input,
the app then creates a new [ASR Stream](../audio-input) and starts
streaming audio data from, e.g., a microphone to Diatheke.

Eventually Diatheke will return an ASR result that includes a transcript
of what the user said ("what's the weather today"). This result is then
used to update the session via the
[Process ASR Result method](./update/#process-asr-result), which will
return the next actions the app should take. In this case, it returns a
[CommandAction](./actions/#command-action) instructing the app to lookup
weather data.

The relevant weather data is then used to update the session again via the
[Process Command Result method](./update/#process-command-result), which
in this example returns two actions the app should take. The first is a
[ReplyAction](./actions/#reply-action), which should be used to create a
new [TTS Stream](../audio-output). Audio from the TTS stream is played
back to the user. Once playback is complete, the next action (another
WaitForUserAction) is handled, and the process continues for the lifetime
of the session.

![Voice Workflow](/img/voice_workflow.png)

### Text I/O
In this example, only text interactions are allowed between the user and
the system. As shown below, the app starts by [creating a session](./create),
which returns a session token and a
[WaitForUserAction](./actions/#wait-for-user-action). In response to this
action, the app uses an appropriate method to get input text from the
user, such a command line prompt or a GUI form.

The text from the user ("what's the weather today") is used to update the
session via the [Process Text method](./update/#process-text), which
returns a [CommandAction](./actions/#command-action) instructing the app
to lookup weather data.

The relevant weather data is then used to update the session again via the
[Process Command Result method](./update/#process-command-result), which
in this example returns two actions the app should take. The first is a
[ReplyAction](./actions/#reply-action), which contains the text of
Diatheke's reply that the app should display to the user using an
appropriate method. The next action (another WaitForUserAction) is then
handled, and the process continues for the lifetime of the session.

![Text Workflow](/img/text_workflow.png)
