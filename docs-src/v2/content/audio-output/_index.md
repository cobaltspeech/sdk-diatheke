---
title: "Audio Output"
description: "Describes how to receive audio output from a session."
weight: 700
---

To synthesize speech for the user from a
[ReplyAction](../sessions/actions/#reply-action), an SDK app may
create a [TTS](../glossary/#tts) stream. Bytes of audio data are sent
on the stream from Diatheke until synthesis is complete, at which
point the stream is closed. The generated audio data will be
formatted and encoded based on the specific TTS model being used for
the session (as defined in the Diatheke server config file).


## Creating the TTS Stream
The TTS stream is created using a [ReplyAction](../sessions/actions/#reply-action).

{{< tabs >}}

{{< tab "Go" "go" >}}
// Create the TTS stream
stream, err := client.NewTTSStream(context.Background(), reply)
{{< /tab >}}

{{< tab "Python" "python">}}
# Create the TTS stream
stream = client.new_tts_stream(reply)
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


## Convenience Functions
The SDK includes some convenience functions to make it easier to
handle the audio data sent on the TTS stream. These typically use a
writer type with the TTS stream created earlier. Apps are more than
welcome to instead use the stream methods directly to
[receive audio](#receiving-audio) if these convenience functions do
not meet their needs.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Set up the audio ouput as an io.Writer
var audioOut io.Writer

// Play the reply uninterrupted
err = diatheke.WriteTTSAudio(stream, audioOut)
{{< /tab >}}

{{< tab "Python" "python">}}
# The audio output should implement a write function, such as a
# file object or process pipe.
audioOut = getAudioOutput()

# Play the reply uninterrupted
diatheke.write_TTS_audio(stream, audioOut)
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


## Receiving Audio
After the stream is created, audio data is sent repeatedly from the
server to the app until synthesis is complete, at which point the
stream is closed by the server.

{{< tabs >}}

{{< tab "Go" "go" >}}
for {
	// Wait for the next audio chunk
	audio, err := stream.ReceiveAudio()
	if err == io.EOF {
		// This indicates that synthesis is complete. No more
		// audio will be coming.
		break
	}

	playAudio(audio)
}
{{< /tab >}}

{{< tab "Python" "python">}}
# Pull messages from the stream as they come.
for data in stream:
    # Play back the audio data.
    play_audio(data.audio)
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
