---
title: "Audio Input"
description: "Describes how to provide audio-based user input to a session."
weight: 244
---

In addition to [pushing text](../push-text), a client may provide
the session input by using audio.

## Creating the Audio Input Stream
To provide the session with audio input, the client code should first
create the input stream. For a given session, only a single audio input
stream should be open at a time, but it perfectly acceptable to open
and close multiple audio streams over the course of a single session.

{{%tabs %}}

{{% tab "Go" %}}
``` go
// Create the stream using the client and session ID
stream, err := client.StreamAudioInput(context.Background(), sessionID)

// OR create the stream using the Session object
stream, err := session.StreamAudioInput(context.Background())
```
{{% /tab %}}

{{% tab "C++" %}}
``` c++
std::unique_ptr<Diatheke::AudioInputStream> stream;

// Create the stream using the client and session ID
stream = client.streamAudioInput(sessionID);

// OR create the stream using the Session object
stream = session.streamAudioInput();
```
{{% /tab %}}

{{% tab "Python" %}}
``` python
# Create the stream using the client and session ID
stream = client.stream_audio_input(session_id)

# OR create the stream using the Session object
stream = session.stream_audio_input()
```
{{% /tab %}}

{{% tab "Swift/iOS" %}}
``` swift
# Create the stream using the client and session ID
let stream = client.streamAudioInput(sessionID: sessionID) { (error) in
        print(error.localizedDescription)
}

# OR create the stream using the Session object
let stream = session.streamAudioInput { (error) in
        print(error.localizedDescription)
}
```
{{% /tab %}}

{{% /tabs %}}



## Pushing Audio

It is the client code's responsibility to handle getting audio data, whether
it is from a microphone, file, or some other source. Audio input should be
formatted and encoded based on the specific [ASR](../../../glossary#asr)
model being used for the session (as defined in the Diatheke server config
file). The audio data may then be sent to the server using the stream
created previously.

{{%tabs %}}

{{% tab "Go" %}}
``` go
// The audio data should be formatted as an array of bytes.
buffer := make([]byte, 8192)

// Get the audio data from a source. This could be a microphone, file, or
// any other source. Here we assume the audio data was retrieved previously
// and stored in our buffer.

// Push the audio data to the input stream. This function may be called
// multiple times.
bytesWritten, err := stream.Write(buffer);

// Be sure to notify Diatheke that no more audio will be coming when we
// are done writing data.
err := stream.Finish();
```
{{% /tab %}}

{{% tab "C++" %}}
``` c++
// Store audio data as a string. Think of the string as an array of chars
// or bytes (a char is one byte of data).
std::string buffer;

// Get the audio data from a source. This could be a microphone, file, or
// any other source. Here we assume the audio data was retrieved previously
// and stored in our buffer.

// Push the audio data to the input stream. This function may be called
// multiple times.
stream->pushAudio(buffer.c_str(), buffer.size());

// Be sure to notify Diatheke that no more audio will be coming when we
// are done writing data.
stream->finished();
```
{{% /tab %}}

{{% tab "Python" %}}
``` python
# Get the audio data from a source. This could be a microphone, file, or
# any other source. Here we assume the audio data was retrieved previously
# and stored in a buffer.

# Push the audio data to the input stream. This function may be called
# multiple times.
stream.write(buffer)

# Be sure to notify Diatheke that no more audio will be coming when
# we are done writing data.
stream.finish()
```
{{% /tab %}}

{{% tab "Swift/iOS" %}}
``` swift
// Get the audio data from a source. This could be a microphone, file, or
// any other source. Here we assume the audio data was retrieved previously
// and stored in a buffer.

// Push the audio data to the input stream. This function may be called
// multiple times.
stream.pushAudio(data: data)

// Be sure to notify Diatheke that no more audio will be coming when
// we are done writing data.
stream.finish()
```
{{% /tab %}}

{{% /tabs %}}

After all audio data has been pushed to the server, client code should be
sure to call the Finish() method to notify Diatheke that no more audio will
be coming.

Transcriptions for session audio input are not returned as part of the
audio input stream. They are returned as
[Recognize events](../event-stream#recognize-event) on the session's
[event stream](../event-stream).
