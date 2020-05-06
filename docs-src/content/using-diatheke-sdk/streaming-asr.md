---
title: "Streaming ASR"
description: "Creating and using an ASR stream unrelated to any running sessions."
weight: 25
---

Not to be confused with a session's [audio input stream](../session/audio-input),
a plain [ASR](../../glossary#asr) stream is used to process audio and
receive a transcript from Diatheke. This functionality is particularly
useful for ASR tasks that fall outside of the normal dialog flow. For
example, an application that wants to allow a user to record a note while
in the middle of a task could use this stream to get a transcript that can
be saved.


Sending audio on an ASR stream does not affect the state of any running
[sessions](../session), or cause the dialog to transition between states.
It essentially forwards the ASR request to the underlying Cubic engine and
forwards the transcription back to the client. The fact that the ASR stream
can be used without a session, makes it particularly useful for debugging
audio issues.


## Creating an ASR Stream
The ASR stream requires a Cubic model (defined in the Cubic server config
file) to be specified at creation. The returned stream will be a
bi-direction stream that allows the client code to push audio to the
server, while concurrently receiving transcripts from the server as they
become available.

There is no limit on the number of ASR streams that may be opened at a
time, but as most the most common use case involves using a single
input source (e.g., microphone), it usually won't make sense to have more
than one ASR stream running at a time.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Specify the Cubic model to use (not a Diatheke model)
cubicModel := "1"

// Create the bi-directional ASR stream
stream, err := client.StreamASR(context.Background(), cubicModel)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Specify the Cubic model to use (not a Diatheke model)
std::string cubicModel = "1";

// Create the bi-directional ASR stream
std::unique_ptr<Diatheke::ASRStream> stream = client.streamASR(cubicModel);
{{< /tab >}}

{{< tab "Python" "python">}}
# Specify the Cubic model to use (not a Diatheke model)
cubic_model = "1"

# Create the bi-directional ASR stream
stream = client.stream_asr(cubic_model)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
# Specify the Cubic model to use (not a Diatheke model)
let cubicModel = "1"

# Create the bi-directional ASR stream
let stream = client.streamASR(model: cubicModel) { (response) in
    // Handle ASR response
    self.handleASRResponse(response)
}
{{< /tab >}}

{{< /tabs >}}


## Pushing Audio
It is the client code's responsibility to handle getting audio data, whether
it is from a microphone, file, or some other source. Audio data should be
formatted and encoded based on the specific [ASR](../../glossary#asr)
model being used for the stream. The audio data is then pushed to the
server as demonstrated below.

After all audio data has been pushed to the server, client code should be
sure to call the AudioFinished() method to notify Diatheke that no more audio will
be coming.

{{< tabs >}}

{{< tab "Go" "go" >}}
// The audio data should be formatted as an array of bytes.
buffer := make([]byte, 8192)

// Get the audio data from a source. This could be a microphone, file, or
// any other source. Here we assume the audio data was retrieved previously
// and stored in our buffer.

// Push the audio data to the stream. This function may be called multiple
// times. It is safe to call concurrently with the stream's Receive()
// method.
if _, err := stream.Write(buffer); err != nil {
    fmt.Printf("Error: %v\n", err)
}

// Be sure to notify Diatheke that no more audio will be coming when we
// are done writing data.
stream.AudioFinished()
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Store audio data as a string. Think of the string as an array of chars
// or bytes (a char is one byte of data).
std::string buffer;

// Get the audio data from a source. This could be a microphone, file, or
// any other source. Here we assume the audio data was retrieved previously
// and stored in our buffer.

// Push the audio data to the stream. This function may be called multiple
// times. It is safe to call concurrently with the stream's waitForResult()
// method.
stream->pushAudio(buffer.c_str(), buffer.size());

// Be sure to notify Diatheke that no more audio will be coming when we
// are done writing data.
stream->finishAudio();
{{< /tab >}}

{{< tab "Python" "python">}}
# Get the audio data from a source. This could be a microphone, file, or
# any other source. Here we assume the audio data was retrieved previously
# and stored in a buffer.

# Push the audio data to the stream. This function may be called multiple
# times. It is safe to call concurrently with the corresponding result_stream.
stream.write(buffer)

# Be sure to notify Diatheke that no more audio will be coming when are
# done writing data.
stream.audio_finished()
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Get the audio data from a source. This could be a microphone, file, or
// any other source. Here we assume the audio data was retrieved previously
// and stored in a buffer.

// Push the audio data to the stream. This function may be called multiple
// times. It is safe to call concurrently with the corresponding result_stream.
stream.pushAudio(data: data)

// Be sure to notify Diatheke that no more audio will be coming when are
// done writing data.
stream.finishAudio()
{{< /tab >}}

{{< /tabs >}}

## Receiving Transcriptions
Transcripts are received over the same ASR stream used to push audio, as
demonstrated below. It is safe to call both PushAudio() and Receive()
concurrently (i.e., on different threads), however it is not safe to
call the same method concurrently. For example, it is an error to call
Receive() for the same stream simultaneously from two threads.

The transcript returned will include the transcript's text and a
confidence score, which is a measure of how confident the ASR engine
is that the transcript matches the supplied audio. The confidence score
will be between 0.0 and 1.0, with 1.0 meaning that the ASR engine is very
confident that the transcript matches what was spoken.

The receiving end of the ASR stream will close after the sending side of
the stream is closed (e.g., by calling AudioFinished()).

{{< tabs >}}

{{< tab "Go" "go" >}}
for {
    // Wait for a transcription from the server. It is safe to call this
    // method concurrently with the stream's Write() method.
    transcript, err := stream.Receive()

    // This indicates the stream has finished, which will happen
    // after the stream's AudioFinished() method is called or the
    // context used to create the stream closes.
    if err == io.EOF {
        break
    }
    if err != nil {
        fmt.Printf("Error: %v\n", err)
        return
    }

    // Display the transcript
    fmt.Printf("ASR Response:\n")
    fmt.Printf("  Transcription: %s\n", transcript.Text)
    fmt.Printf("  Confidence Score: %v\n\n", transcript.ConfidenceScore)
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Wait for a transcription from the server. It is safe to call this
// method concurrently with the stream's pushAudio() method. The
// waitForResult() method will return false after the stream's
// finishAudio() method has been called.
cobaltspeech::diatheke::ASRResponse result;
while (stream->waitForResult(&result))
{
    // Display the transcript
    std::cout << "ASR Response:" << std::endl;
    std::cout << "  Transcription: " << result.text() << std::endl;
    std::cout << "  Confidence Score: " << result.confidence_score()
              << std::endl
              << std::endl;
}
{{< /tab >}}

{{< tab "Python" "python">}}
# Wait for a transcription from the server. It is safe to do this
# concurrently with the stream's write() method. The for loop
# will continue to iterate until the stream's audio_finished() method
# has been called.
for result in stream.result_stream:
    # Display the transcript
    print("ASR Response:")
    print("  Transcript: " + result.text)
    print("  Confidence Score: {}\n".format(result.confidence_score))
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func handleASRResponse(_ response: Cobaltspeech_Diatheke_ASRResponse) {
    print("ASR Response:")
    print("Transcription: \(response.text)")
    print("Confidence Score: \(response.confidenceScore)")
}
{{< /tab >}}

{{< /tabs >}}
