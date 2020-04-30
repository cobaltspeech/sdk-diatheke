---
title: "Streaming TTS"
description: "Creating and using a TTS stream unrelated to any running sessions."
weight: 26
---


Not to be confused with a session's [audio output stream](../session/audio-output),
a plain [TTS](../../glossary#tts) stream is used to synthesize speech
unrelated to any currently running [sessions](../session) or Diatheke
models. This means that, unlike audio output from a session which is
sent to the client in response to user input as defined in the model,
the audio output for a plain TTS stream is returned to the client
immediately after the TTS request is made.

This functionality is provided mainly as a convenience. There may be
some situations where it is useful to synthesize audio unrelated to a
running session (such as debugging). For the majority of situations,
Cobalt recommends embedding TTS replies in the Diatheke model itself,
so that they may be returned as part of the normal dialog flow for a
session.

## Creating the TTS Stream
The TTS stream requires a Luna model (defined in the Luna server config
file) and the text to synthesize to be specified at creation. The
returned stream will receive audio data from the server until synthesis
is complete and the stream is closed.

{{%tabs %}}

{{% tab "Go" %}}
``` go
// Specify the Luna model to use (not a Diatheke model)
lunaModel := "1"
sentence := "this is the text to synthesize"

// Create the TTS stream
stream, err := client.StreamTTS(context.Background(), lunaModel, sentence)
```
{{% /tab %}}

{{% tab "C++" %}}
``` c++
// Specify the Luna model to use (not a Diatheke model)
std::string lunaModel = "1";
std::string sentence = "this is the text to synthesize";

// Create the TTS stream
std::unique_ptr<Diatheke::TTSStream> stream = 
    client.streamTTS(lunaModel, sentence);
```
{{% /tab %}}

{{% tab "Python" %}}
``` python
# Specify the Luna model to use (not a Diatheke model)
luna_model = "1"
sentence = "this is the text to synthesize"

# Create the TTS stream
stream = client.stream_tts(luna_model, sentence)
```
{{% /tab %}}

{{% tab "Swift/iOS" %}}
``` swift
// Specify the Luna model to use (not a Diatheke model)
let lunaModel = "1"
let sentence = "this is the text to synthesize"

// Create the TTS stream
client.streamTTS(model: lunaModel, text: sentence) { (response) in
    // Handle TTS response
    self.handleTTSResponse(response)
}
```
{{% /tab %}}

{{% /tabs %}}

## Receiving Audio
The only data returned on the TTS stream will be audio data. The simplest
way to receive it is to set up a loop to receive and process the data
until the stream is closed, as shown below.

The TTS generated audio data will be formatted and encoded based on the
specific TTS model being used for the stream. It is the client code's
responsibility to handle the output audio, whether it sends it to an output
device (e.g., speakers), saves it to a file, or does some other processing
of the audio data.

{{% tabs %}}

{{% tab "Go" %}}
``` go
// Receive data from the TTS stream until it is closed, which will happen
// when synthesis is complete, or the context used to create the stream
// is cancelled.
for {
    // Wait for data from the server
    msg, err := stream.Recv()

    // This indicates the stream has finished, which will happen
    // when the session ends or the context used to create the stream
    // closes.
    if err == io.EOF {
        break
    }
    if err != nil {
        fmt.Printf("Error: %v\n", err)
        return
    }

    // Use the audio data, which will be an array of bytes.
    fmt.Printf("TTS Data size (bytes): %v\n", len(msg.Data))
}

fmt.Printf("Synthesis complete.\n")
```
{{% /tab %}}

{{% tab "C++" %}}
``` c++
// Receive data from the TTS stream until it is closed, which will happen
// when synthesis is complete.
cobaltspeech::diatheke::TTSResponse response;
while (stream->waitForAudio(&response))
{
    // Use the audio data, which will be given as a string. Think of it as an
    // array of chars (a char is one byte).
    std::string audioData = response.data();
    std::cout << "TTS Data size (bytes): " << audioData.size() << std::endl;
}

std::cout << "Synthesis complete." << std::endl;
```
{{% /tab %}}

{{% tab "Python" %}}
``` python
# Receive data from the TTS stream until it is closed, which will happen
# when synthesis is complete.
for response in stream:
    # Use the audio data
    audio_data = response.data
    print("TTS Data size (bytes): {}".format(len(audio_data)))

print("Synthesis complete.")
```
{{% /tab %}}

{{% tab "Swift/iOS" %}}
``` swift
func handleTTSResponse(_ response: Cobaltspeech_Diatheke_TTSResponse) {
    print("TTS Data size (bytes): \(response.data.count)")
}
```
{{% /tab %}}

{{% /tabs %}}
