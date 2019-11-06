# Go Examples

This directory contains examples demonstrating how to use the
Diatheke SDK.

## Build
The code for the demo executables is found in the cmd directory,
with each demo having its own subdirectory. To build a demo,
simply use the `go build` command, with the specific demo's path
specified. For example,

```bash
go build ./cmd/demo-asr
go build ./cmd/demo-audio
go build ./cmd/demo-cli
go build ./cmd/demo-tts
```

## Run

Each demo illlustrates a different aspect of the Diatheke SDK. While the
demos separate these functions for the sake of simplicity, there is nothing
restricting a developer from mixing all of them in the same application.

Each demo is terminal based and relies on keyboard input in some form,
whether it be text input, or merely pressing the Enter key to start and
stop audio recording. As such, they should all be launched from a terminal
window. Each demo may be terminated by using the `Ctrl+D` shortcut.

The four demos are
* [demo-audio](#demo-audio) - Creates a dialog using audio I/O.
* [demo-cli](#demo-cli) - Creates a text-based dialog
* [demo-asr](#demo-asr) - Demonstrates Automatic Speech Recognition (ASR).
* [demo-tts](#demo-tts) - Demonstrates Text-to-Speech (TTS).

### Config File
Each demo application requires a configuration file to be specified. 
An example config file with documentation about each parameter in the file
is provided in `examples/config.sample.toml`. The same config file will
work for all the demos.

### Audio I/O
For demos that use audio, the audio I/O is handled exclusively by 
external applications such as 
[aplay/arecord](https://linux.die.net/man/1/arecord) and
[sox](http://sox.sourceforge.net/). The specific application can be
anything as long the following conditions are met:
* The application supports the encodings, sample rate, bit-depth, etc. 
  required by the underlying Cubic ASR and Luna TTS models.
* For recording, the application must stream audio data to stdout.
* For playback, the application must accept audio data from stdin.

The specific applications (and their args) should be specified in 
the [configuration file](#config-file).

## demo-audio
```bash
# Run the audio-based dialog demo
./demo-audio -config <path/to/config.toml>
```

This application demonstrates how to setup an audio-based conversation with
Diatheke. This demo requires that both recording and playback applications
be specified in the [config file](#config-file). Audio recording is toggled
by pressing the Enter key. When recording, a user may speak an appropriate
phrase (defined by the Diatheke model being used), and Diatheke will respond
with TTS generated audio.

## demo-cli
```bash
# Run the text-based dialog demo
./demo-cli -config <path/to/config.toml>
```

This application demonstrates how to setup a text-based conversation with
Diatheke. A user simply sends text to Diatheke at the prompt, and Diatheke
will return a text response. The Diatheke model defines the specific text
that a user is allowed to submit, as well as the replies that come back
from Diatheke.

## demo-asr
```bash
# Run the streaming ASR demo
./demo-asr -config <path/to/config.toml>
```

This application demonstrates how to run Automatic Speech Recognition (ASR)
that is unrelated to any running conversations in Diatheke. This demo
requires a recording application to be specified in the
[config file](#config-file). Audio recording is toggled by pressing the
Enter key. While recording, a user may speak any phrase that is supported
by the underlying Cubic ASR model. Diatheke will forward the transcription
from Cubic back to the client, which will then be printed to the terminal
for the user to see.

## demo-tts
```bash
# Run the streaming TTS demo
./demo-tts -config <path/to/config.toml>
```
This application demonstrates how to run Text-to-Speech (TTS) that is
unrelated to any running conversations in Diatheke. This demo requires
a playback application to be specified in the [config file](#config-file).
While running, a user may submit any text at the prompt, which Diatheke
will forward to the underlying Luna TTS engine. The generated speech will
be returned to the client and played through the external playback
application.

*Tip: When the submitted text includes numbers, you can avoid 
unexpected results by spelling out the number. For example, the number
1804 could be spelled as "one thousand eight hundred and four",
"eighteen hundred and four", "eighteen oh four", or even "one eight zero
four", depending on context and preference.*
