---
title: "Diatheke SDK"
weight: 1
---

Diatheke is Cobalt's dialog management engine. It uses a combination of
speech technologies and artificial intelligence to allow users to interact
with computers and mobile devices through audio and text based dialogs.


## Audio Based Dialogs
Audio based dialog management uses multiple speech technologies, including
[Automatic Speech Recognition (ASR)](glossary#asr),
[Natural Language Understanding (NLU)](glossary#nlu), and
[Text To Speech (TTS)](glossary#tts). It starts with audio input from a
client application (human speech), which Diatheke sends to the ASR engine
to get a transcription. This transcription is passed on to the NLU engine,
which converts the transcription into an [intent](glossary#intent) and
[entities](glossary#entity). The intent and entites are then used to
perform an [action](glossary#action), as defined by the Diatheke model.
One such action is for Diatheke to send a reply to the user. The reply text
is sent to the TTS engine, which then synthesizes audio to send back to the
client, as shown below.

{{<mermaid align="center">}}
graph LR;
    subgraph Diatheke
        A[ASR] -->|Transcription| B[NLU]
        B -->|Intents and Entities| C[Dialog</br>Model]
        C -->|Reply| D[TTS]
    end
    E[Client</br>Application] ==>|Audio Input| A
    D ==>|Synthesized Audio| E
{{< /mermaid >}}


## Text Based Dialogs
Diatheke is also capable of processing dialogs without using audio input or
output. In this case, text is sent to Diatheke, which forwards it directly
to the NLU engine and converts the text to an intent and entities, as it
did with the transcription in the audio workflow. The intent and entities
are used to perform an action, such as sending a reply to the client
application in the form of text, as shown below.

{{<mermaid align="center">}}
graph LR;
    subgraph Diatheke
        A[NLU] -->|Intents and Entities| B[Dialog</br>Model]
    end
    C[Client</br>Application] ==>|Text Input| A
    B ==>|Reply Text| C
{{< /mermaid >}}
