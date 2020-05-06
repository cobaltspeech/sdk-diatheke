---
title: "Audio Output"
description: "Describes how to receive audio output from a session."
weight: 245
---

The audio output stream is the audio version of the
[Reply event](../event-stream#reply-event). When Diatheke wants to reply
to user input, this stream will receive notifications with the text of
the reply, and the [TTS](../../../glossary#tts) generated audio bytes.


## Creating the Audio Reply Stream
Before any audio for the session can be received, the client code should
first create the audio reply stream. Similar to the event stream, this
stream will be closed by the server when the session ends.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Create the stream using the client and session ID
stream, err := client.StreamAudioReplies(context.Background(), sessionID)

// OR create the stream using the Session object
stream, err := session.StreamAudioReplies(context.Background())
{{< /tab >}}

{{< tab "C++" "c++" >}}
// The stream is returned as a std::unique_ptr
std::unique_ptr<Diatheke::AudioReplyStream> stream;

// Create the stream using the client and session ID
stream = client.streamAudioReplies(sessionID);

// OR create the stream using the Session object
stream = session.streamAudioReplies();
{{< /tab >}}

{{< tab "Python" "python">}}
# Create the stream using the client and session ID
stream = client.stream_audio_replies(session_id)

# OR create the stream using the Session object
stream = session.stream_audio_replies()
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
# Create the stream using the client and session ID
let stream = client.streamAudioReplies(sessionID: sessionID) { (reply) in
    // Handle audio reply
}

# OR create the stream using the Session object
let stream = session.streamAudioReplies { (reply) in
    // Handle audio reply
}
{{< /tab >}}

{{< /tabs >}}


## Receiving Audio
A single reply from Diatheke will have multiple messages on the audio reply
stream. First, it will receive a message containing the text of the reply.
Then, it will receive one or more messages containing the 
[TTS](../../../glossary#TTS) generated audio data. Lastly, it will receive
an empty message that contains no data, but serves as an indicator that the
reply has finished TTS generation and that no more data will be coming for
the current reply. An example showing how to handle these cases is given
below.

The TTS generated audio data will be formatted and encoded based on the
specific TTS model being used for the session (as defined in the Diatheke
server config file). It is the client code's responsibility to handle
the output audio, whether it sends it to an output device (e.g., speakers),
saves it to a file, or some other processing of the audio data.

{{< tabs >}}

{{< tab "Go" "go" >}}
for {
    // Wait for replies from the server until the stream is closed,
    // which will happen when the session is closed or the stream's
    // context ends.
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

    // Check which message type we have received.
    switch reply := msg.OutputMessage.(type) {
    case *diathekepb.AudioReply_Text:
        // The text of the reply comes first for a reply from Diatheke.
        fmt.Printf("Text: %s\n", reply.Text)

    case *diathekepb.AudioReply_Data:
        // Audio data is received until speech synthesis is done.
        fmt.Printf("Data size (bytes): %v\n", len(reply.Data))

    case *diathekepb.AudioReply_End:
        // This message comes at the end of a reply, after speech synthesis
        // is complete. It has no data.
        fmt.Printf("Reply complete\n")

    default:
        fmt.Printf("received unexpected AudioReply type\n")
    }
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Wait for replies until the stream is closed, which will happen when
// the session is closed or the stream's context stops.
cobaltspeech::diatheke::AudioReply reply;
while (stream->waitForReply(&reply))
{
    // Check which message type we have received
    if (reply.has_text())
    {
        // The text of the reply comes first for a reply from Diatheke.
        std::cout << "Text: " << reply.text() << std::endl;
    }
    else if (reply.has_data())
    {
        // Audio data is received until speech synthesis is done.
        // It is returned as a string (think of it as an array of
        // chars/bytes).
        std::string audioData = reply.data();
        std::cout << "Data size (bytes): " << audioData.size() << std::endl;
    }
    else if (reply.has_end())
    {
        // This message comes at the end of a reply, after speech synthesis
        // is complete. It has no data.
        std::cout << "Reply complete" << std::endl;
    }
    else
    {
        std::cerr << "received unexpected AudioReply type" << std::endl;
    }
}

stream->close();
{{< /tab >}}

{{< tab "Python" "python">}}
# Wait for replies from the server until the stream is closed, which
# will happen when the session is closed.
for msg in stream:
    # Check which message type we have received
    if msg.HasField("text"):
        # The text of the reply comes first for a reply from Diatheke
        print("Text: " + reply.text)

    elif msg.HasField("data"):
        # Audio data is received until speech synthesis is done.
        print("Data size (bytes): {}".format(len(reply.data)))

    elif msg.HasField("end"):
        # This message comes at the end of a reply, after speech synthesis
        # is complete. It has no data.
        print("Reply complete")

    else:
        print("Received unexpected AudioReply type")
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Handle replies from the server until the stream is closed, which
// will happen when the session is closed.
let stream = session.streamAudioReplies { (reply) in
    guard let outputMessage = reply.outputMessage else { return }
    
    switch outputMessage {
    case .text(let text):
        print("Text: \(text)")
    case .data(let audioData):
        print("Data size (bytes): \(audioData.count)")
    case .end(_):
        print("Reply complete")
    }
}
{{< /tab >}}

{{< /tabs >}}
