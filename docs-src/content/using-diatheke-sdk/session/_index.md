---
title: "Sessions"
description: "Defines what a session is, and how to interact with it."
weight: 24
---

A session represents all the components ([ASR](../../glossary/#asr),
[TTS](../../glossary/#tts), [NLU](../../glossary/#nlu), etc.) necessary
to carry on a conversation with Diatheke. A single session keeps track
of a dialog's current state, with all possible states being defined by
a Diatheke model. The session transitions between states based on user
input, which can be either [text](push-text) or [audio](audio-input).
In response to user input, the Diatheke server will notify the client of
[events](event-stream) that occur as part of the transition, including
[TTS generated audio](audio-output) and requests to execute 
[commands](event-stream#command-event).

The following sections describe how to use use a session in the SDK.

## Contents
{{% children  %}}
