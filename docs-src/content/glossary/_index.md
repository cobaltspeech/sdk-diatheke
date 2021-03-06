---
title: "Glossary"
weight: 1001
---

### Action
When a user says something, it is converted to [intents](#intent) and
[entities](#entity). Then, the system can take any *action* in response.
Actions can include [text-to-speech](#tts) output, or running a specific
[command](#command).

For example, if someone asks to pay money off their credit card, then
the system should interact with the banking backend to pay the card, while
also playing a TTS response to confirm to the user what is happening.


### ASR
The first step in any dialogue system is 
[Automatic Speech Recognition](https://en.wikipedia.org/wiki/Speech_recognition)
(ASR), which converts the audio of someone's [utterance](#utterance) into
text. ASR usually outputs in 'spoken form', i.e. numbers and other expressions
are in words, not digits ("three hundred and twenty one" and not "321"),
but automatic formatting can optionally be applied.

Diatheke uses Cobalt's ASR engine, Cubic, to process audio and supply
transcripts.


### Command
A command is a specific type of [action](#action) that executes some set
of tasks in response to a recognized [intent](#intent). For example, if
the [utterance](#utterance) "Play some music" was recognized as an intent,
the command might handle the actual playback of an audio file.


### Entity
An entity (a.k.a. a slot) appears in an [utterance](#utterance).
It represents a value in an [intent](#intent) that can change between 
utterances or even be omitted entirely in some cases. Entities are
defined in the Diatheke model.

{{% panel theme="default" header="Examples" %}}
1. "What's the weather forecast for
  <mark style="background-color: SlateBlue; color: white">Paris</mark>?" </br>

2. "Book me <mark style="background-color: DodgerBlue; color: white">three</mark>
  <mark style="background-color: Orange;">plane</mark> tickets from
  <mark style="background-color: SlateBlue; color: white">London</mark> to
  <mark style="background-color: Violet;">New York</mark>, please." </br>

3. "Play the song <mark style="background-color: SeaGreen; color: white">New York</mark>" </br>

4. "Pay <mark style="background-color: DodgerBlue; color: white">three</mark>
  pounds off my credit card." </br>
{{% /panel %}}

The first two examples have entities that are cities - the first has a single
<mark style="background-color: SlateBlue; color: white">City</mark>,
 while the second might have a 
<mark style="background-color: SlateBlue; color: white">SourceCity</mark> and
<mark style="background-color: Violet;">DestinationCity</mark>.

The second and fourth examples have entities that are 
<mark style="background-color: DodgerBlue; color: white">numbers</mark>,
though they are interpreted differently in the two utterances. One is a quantity,
and the other is an amount of money.

The same words can mean different entities in different contexts. E.g. 'New York'
could be either a 
<mark style="background-color: Violet;">City</mark> or a
<mark style="background-color: SeaGreen; color: white">SongName</mark>,
depending on the context.


### Intent
Intent is the name for a group of [utterances](#utterance) which mean the
same sort of thing and result in a particular [action](#action) being taken.
Intents are defined as part of the Diatheke model.

{{% panel theme="default" header="Example - GetWeatherForecastIntent" %}}
"What's the weather forecast for Paris?" </br>
"What's the weather like in London?" </br>
"Is it going to be sunny today?" </br>
{{% /panel %}}

{{% panel theme="default" header="Example - BookTicketIntent" %}}
"Book me three plane tickets from London to New York, please." </br>
"Book me two train tickets from Newcastle to Edinburgh." </br>
"Book me five bus tickets to Grand Junction." </br>
{{% /panel %}}


### NLU
[Natural Language Understanding](https://en.wikipedia.org/wiki/Natural-language_understanding)
(NLU) is a field within artificial intelligence that deals with machine
reading comprehesion. It attempts to discern the meaning of (sometimes
incomplete) sentences. In Diatheke, these interpretations are the
[intent](#intent) with its associated [entites](#entity).


### Session
A session represents all the components ([ASR](#asr), [TTS](#tts),
[NLU](#nlu), etc.) necessary to carry on a conversation with Diatheke.
A single session keeps track of a dialog's current state, with all
possible states being defined by a Diatheke model.


### Slot
See [Entity](#entity)


### Story
In a Diatheke model, a story groups together several related [actions](#action)
that help accomplish a particular goal. This is where the desired dialog
flow is defined as sequences of actions, such as waiting for user input,
executing commands, and sending the user replies. A Diatheke model includes
at least a "main" story that defines where a [session](#session) begins,
and can have any number of additional stories that are used during a session.

### TTS
[Text To Speech](https://en.wikipedia.org/wiki/Speech_synthesis) (TTS) synthesizes 
audio that replicates human speech from written text.

Diatheke uses Cobalt's TTS engine, Luna, to process audio and supply
transcripts.


### Utterance
An *utterance* is something that someone says - it isn't necessarily 
a full sentence as people don't always speak grammatically.

{{% panel theme="default" header="Examples" %}}
"What's the weather forecast?" </br>
"Book me a train ticket, please" </br>
"Play some music" </br>
"Pay my credit card balance in full" </br>
"Cancel" </br>
{{% /panel %}}
