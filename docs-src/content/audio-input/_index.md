---
title: "Audio Input"
description: "Describes how to provide audio-based user input to a session."
weight: 600
---

To process a user's speech, the calling application may create an 
[ASR](../glossary/#asr) stream. Bytes of audio data are sent to the
stream and the server returns ASR results, which include a transcript
of the speech, until the stream is closed. The audio data encoding must
match what is expected by the server. The sample rate for any given
model is provided with the model information returned by the
[ListModels](../connecting/#list-models) method. Currently, Diatheke
supports two different ASR streams:

* [StreamASR](#streamasr) - used to handle a conversation turn with Diatheke.
* [Transcribe](#transcribe) - used to get an application specific 
  transcript not intended to control the dialog.

For simplified usage, take a look at the [convenience functions](#convenience-functions).

## StreamASR
This method is used to handle a single conversation turn with Diatheke, where
the user is expected to provide input before Diatheke continues to the next
action. It includes functionality for [wake word](#wake-word) detection
and speech processing in the context of the current dialog state.
Unlike the Transcribe stream, this stream will only ever return a single
ASR result, which may be used to
[update a session](../sessions/update/#process-asr-result), at which point
the stream is closed and a new stream is created for the next [Wait For User
action](../sessions/actions/#wait-for-user-action).

### Creating the ASR Stream
Use the [session token](../sessions/create) to create the ASR stream.

Developers who are creating their own bindings for the
[gRPC interface](../protobuf/) should note that the first message sent
on the stream returned by the `StreamASR` method should always be the
session token.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Create the ASR stream. This automatically sends the session
// token first on the new stream.
stream, err := client.NewSessionASRStream(context.Background(), session.Token)
{{< /tab >}}

{{< tab "Python" "python">}}
# Create the ASR stream. This automatically sends the session
# token first on the new stream.
stream = client.new_session_asr_stream(session.token)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Create the ASR stream. This automatically sends the session
// token first on the new stream.
Diatheke::ASRStream stream = client.newSessionASRStream(session.token());
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Create the ASR stream. This automatically sends the session
// token first on the new stream.
self.asrStream = client.newSessionASRStream(token: token, asrResultHandler: { (result) in
	switch result {
	case .success(let asrResult):
		// Update the session with the ASR Result
		self.client.processASRResult(token: self.token, asrResult: asrResult) { (sessionOutput) in
			self.processActions(sessionOutput: sessionOutput)
		} failure: { (error) in
			print(error.localizedDescription)
		}
	case .failure(let error):
		print("ASR result error received: \(error)")
	}
}, completion: { (error) in
	if let error = error {
		print(error.localizedDescription)
	}
})
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming!
// For now, please see https://grpc.io/docs/platforms/android/java/basics/#client-side-streaming-rpc
{{< /tab >}}

{{< /tabs >}}


### Sending Audio
After the stream is created, audio data is sent repeatedly over the
stream until the stream is closed or there is no more audio to send.

{{< tabs >}}

{{< tab "Go" "go" >}}
var audioData []byte
for haveAudioData {
	audioData = getFromSource()

	// Send the audio bytes
	if err := stream.SendAudio(audioData); err == io.EOF {
		// The stream has been closed, and a result is available.
		break
	}
}
{{< /tab >}}

{{< tab "Python" "python">}}
while haveAudioData:
    audioData = getFromSource()

    # Send the audio bytes
    if not stream.send_audio(audioData):
        # The stream has been closed, and a result is available
        break
{{< /tab >}}

{{< tab "C++" "c++" >}}
while (haveAudioData) {
    std::string audioData = getFromSource();

    // Send the audio bytes
    if (!stream.sendAudio(audioData)) {
        // The stream has been closed, and a result is available
        break;
    }
}
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Send the audio bytes
asrStream.sendAudio(data: data, completion: { (error) in
	if let error = error {
        	print(error.localizedDescription)
	}
})
{{< /tab >}}

{{< /tabs >}}


### ASR Result
When there is no more audio data to send, or if the ASR stream was
closed by the server, the application may retrieve the ASR result, which
includes a transcript of what was spoken. This result may be used to
[update a session](../sessions/update/#process-asr-result).

{{< tabs >}}

{{< tab "Go" "go" >}}
// Get the result from the stream. If the stream has not already
// been closed by the server, this will close it.
result, err := stream.Result()

// The transcript
fmt.Printf("transcript: %v\n", result.Text)

// The confidence the ASR system has in the given transcript,
// given as a value between 0 and 1.0, with 1.0 being the most
// confident in the result.
fmt.Printf("confidence: %v\n", result.Confidence)
{{< /tab >}}

{{< tab "Python" "python">}}
# Get the result from the stream. If the stream has not already
# been closed by the server, this will close it.
result = stream.result()

# The transcript
print("transcript:", result.text)

# The confidence the ASR system has in the given transcript,
# given as a value between 0 and 1.0, with 1.0 being the most
# confident in the result.
print("confidence:", result.confidence)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Get the result from the stream. If the stream has not already
// been closed by the server, this will close it.
auto result = stream.result();

// The transcript
std::cout << "transcript: " << result.text() << std::endl;

// The confidence the ASR system has in the given transcript,
// given as a value between 0 and 1.0, with 1.0 being the most
// confident in the result.
std::cout << "confidence: " << result.confidence() << std::endl;
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// ASR Results come to asrResultHandler block on creating ASRStream
self.asrStream = client.newSessionASRStream(token: token, asrResultHandler: { (result) in
	switch result {
	case .success(let asrResult):
		// The transcript
		print("Transcript: \(asrResult.text)")
		// The confidence the ASR system has in the given transcript,
		// given as a value between 0 and 1.0, with 1.0 being the most
		// confident in the result.
		print("Confidence: \(asrResult.confidence)")
	case .failure(let error):
		print("ASR result error received: \(error)")
	}
}, completion: { (error) in
	if let error = error {
		print(error.localizedDescription)
	}
})

// When there is no more audio data to send
// (i.e. when the app needs to stop recording audio)
// call ASRStream's result() method to close the ASR stream
self.asrStream.result(completion: { (error) in
	if let error = error {
		print(error.localizedDescription)
	}
})
{{< /tab >}}

{{< /tabs >}}

## Transcribe
This method is used to get a general purpose transcription from Diatheke.
Unlike the StreamASR method, the transcripts returned on this stream are
not used to update the session. Their purpose is application specific,
such as taking a note, sending a text message, etc. This stream
does not use [wake word](#wake-word) detection to begin the transcription,
but it may have conditions defined in the Diatheke model to end the
transcription, such as a non-speech timeout (i.e, speech not detected for
a specified duration), or a sleep word (i.e, a word or phrase to indicate
that the transcription should end). A single Transcribe stream may return
multiple [results](#transcribe-results) before it is finished.

### Creating the Transcribe Stream
Use the [Transcribe action](../sessions/actions/#transcribe-action)
to create the Transcribe stream.

Developers who are creating their own bindings for the
[gRPC interface](../protobuf/) should note that the first message sent
on the stream returned by the `Transcribe` method should always be the
transcribe action.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Create the transcribe stream. This automatically sends the transcribe
// action on the stream first.
stream, err := client.NewTranscribeStream(context.Background(), action)
{{< /tab >}}

{{< tab "Python" "python">}}
# Create the transcribe stream. This automatically sends the transcribe
# action on the stream first.
stream = client.new_transcribe_stream(action)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Create the transcribe stream. This automatically sends the transcribe
// action on the stream first.
Diatheke::TranscribeStream stream = client.newTranscribeStream(action);
{{< /tab >}}

{{< /tabs >}}

### Sending Audio
After the stream is created, audio data is sent repeatedly over the
stream until the stream is closed or there is no more audio to send.
Because the Transcribe stream is bidirectional, this must be done
concurrently with receiving results. For most programming languages,
this means it should be handled on a separate thread.

{{< tabs >}}

{{< tab "Go" "go" >}}
var audioData []byte
for haveAudioData {
	audioData = getFromSource()

	// Send the audio bytes
	if err := stream.SendAudio(audioData); err == io.EOF {
		// The stream has been closed, and a result is available.
		break
	}
}

// Be sure to close the stream to notify the server that no more
// audio will be coming.
err = stream.SendFinished()
{{< /tab >}}

{{< tab "Python" "python">}}
while haveAudioData:
    audioData = getFromSource()

    # Send the audio bytes
    if not stream.send_audio(audioData):
        # The stream has been closed
        break

# Be sure to close the stream to notify the server that no more
# audio will be coming.
stream.send_finished()
{{< /tab >}}

{{< tab "C++" "c++" >}}
while (haveAudioData) {
    std::string audioData = getFromSource();

    // Send the audio bytes
    if (!stream.sendAudio(audioData)) {
        // The stream has been closed
        break;
    }
}

// Be sure to close the stream to notify the server that no more
// audio will be coming.
stream.sendFinished();
{{< /tab >}}

{{< /tabs >}}

### Transcribe Results
Multiple results may be sent on the receiving half of the Transcribe
stream during its lifetime. Some of these results are partial, which
indicate that the result may change with further processing. Partial
results are mainly useful for displaying progress to a user in a
graphical user interface if one is being used. Non-partial results
contain the final transcript that is of interest to the application.
Note that multiple non-partial results may be received by the
application before the stream ends.

Due to the bidirectional nature of the Transcribe stream, results must
be processed concurrently with sending audio data. This may be done
on any thread, and it is often done on the main thread, as many
applications don't wish to continue processing until the transcription
is complete. However, it is not a strict requirement that results be
processed on the main thread, and an application should adopt an
architecture that makes the most sense for its use case.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Process results as they come.
for {
	result, err := stream.ReceiveResult()
	if err == io.EOF {
		// The stream has closed
		return
	} else if err != nil {
		// Handle the error
		return
	}

	// Boolean indicating whether the result is partial (i.e., may
	// change with further processing).
	if result.IsPartial {
		// Print the partial transcript.
		fmt.Printf("partial transcript: %v\n", result.Text)
	} else {
		// Print the finalized transcript.
		fmt.Printf("transcript: %v\n", result.Text)
		
		// The confidence the ASR system has in the given transcript,
		// given as a value between 0 and 1.0, with 1.0 being the most
		// confident in the result. Note that partial results won't
		// have an accurate confidence score.
		fmt.Printf("confidence: %v\n", result.Confidence)
	}
}
{{< /tab >}}

{{< tab "Python" "python">}}
# Process results as they come.
while True:
    result = stream.receive_result()
    if result is None:
        # The stream has closed
        break

    # Boolean indicating whether the result is partial (i.e., may
    # change with further processing).
    if result.is_partial:
        # Print the partial transcript.
        print("partial transcript:", result.text)
    else:
        # Print the finalized transcript.
        print("transcript:", result.text)

        # The confidence the ASR system has in the given transcript,
        # given as a value between 0 and 1.0, with 1.0 being the most
        # confident in the result. Note that partial results won't
        # have an accurate confidence score.
        print("confidence:", result.confidence)
{{< /tab >}}

{{< tab "C++" "c++" >}}
while (true) {
    cobaltspeech::diatheke::TranscribeResult result;
    if (!stream.receiveResult(&result)) {
        // The stream has closed
        break;
    }

    // Boolean indicating whether the result is partial (i.e., may
    // change with further processing).
    if (result.is_partial()) {
        // Print the partial transcript.
        std::cout << "partial transcript: " << result.text() << std::endl;
    } else {
        // Print the finalized transcript.
        std::cout << "transcript: " << result.text() << std::endl;

        // The confidence the ASR system has in the given transcript,
        // given as a value between 0 and 1.0, with 1.0 being the most
        // confident in the result. Note that partial results won't
        // have an accurate confidence score.
        std::cout << "confidence: " << result.confidence() << std::endl;
    }
}
{{< /tab >}}

{{< /tabs >}}


## Convenience Functions
The SDK includes some convenience functions to make it easier to send
audio data to Diatheke and get results. These typically use a reader
type with the ASR stream created earlier. Applications are more than welcome
to instead use the stream methods directly to [send audio](#sending-audio)
and [get a result](#asr-result) if these convenience functions do not
meet their needs.

### ReadASRAudio
{{< tabs >}}

{{< tab "Go" "go" >}}
// Set up the audio source as an io.Reader
var audioSource io.Reader

// Send up to 8kB chunks of audio at a time
buffSize := 8192

// Process audio until we get a result or the audioSource returns
// io.EOF, whichever comes first.
result, err := diatheke.ReadASRAudio(stream, audioSource, buffSize)
fmt.Printf("transcript: %v\n", result.Text)
{{< /tab >}}

{{< tab "Python" "python">}}
# The audio source should implement a read(size) function, such as
# a file object or process pipe.
audioSource = getAudioSource()

# Send up to 8kB chunks of audio at a time
buffSize = 8192

# Process audio until we get a result or the reader returns empty
# bytes (EOF), whichever comes first. Note that for Python, this
# convenience function does not take a stream, but instead uses
# the client object directly.
result = client.read_asr_audio(session.token, audioSource, buffSize)
print("transcript:", result.text)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// The audio source should inherit from the Diatheke::AudioReader class.
Diatheke::AudioReader* audioSource;

// Send up to 8kB chunks of audio at a time
size_t buffSize = 8192;

// Process audio until we get a result or the number of bytes read
// (as returned by the Diatheke::AudioReader) is zero.
cobaltspeech::diatheke::ASRResult result =
    Diatheke::ReadASRAudio(stream, audioSource, buffSize);
std::cout << "transcript: " << result.text() << std::endl;
{{< /tab >}}

{{< /tabs >}}

### ReadTranscribeAudio
{{< tabs >}}

{{< tab "Go" "go" >}}
// Set up the audio source as an io.Reader
var audioSource io.Reader

// Send up to 8kB chunks of audio at a time
buffSize := 8192

// Set up a result callback
handler := func(result *diathekepb.TranscribeResult) {
	// Do something interesting with the result
	fmt.Printf("result: %+v\n", result)
}

// Call the convenience function, which will block until the audio source
// returns io.EOF or the stream closes.
err := diatheke.ReadTranscribeAudio(stream, audioSource, buffSize, handler)
{{< /tab >}}

{{< tab "Python" "python">}}
# The audio source should implement a read(size) function, such as
# a file object or process pipe.
audioSource = getAudioSource()

# Send up to 8kB chunks of audio at a time
buffSize = 8192

# Set up a result callback
def handler(result):
    # Do something interesting with the result
    print("result text:", result.text)

# Call the convenience function, which will block until the audio
# source returns EOF or the stream closes. Note that for Python, this
# convenience function does not take a stream, but instead uses
# the client object directly.
client.read_transcribe_audio(action, audioSource, buffSize, handler)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// The audio source should inherit from the Diatheke::AudioReader class.
Diatheke::AudioReader* audioSource;

// Send up to 8kB chunks of audio at a time
size_t buffSize = 8192;

// Set up a result callback
auto handler = [](const cobaltspeech::diatheke::TranscribeResult &result) {
    // Do something interesting with the result.
    std::cout << "result text: " << result.text() << std::endl;
}

// Call the convenience function, which will block until the audio
// source returns EOF or the stream closes.
Diatheke::ReadTranscribeAudio(stream, audioSource, buffSize, handler);
{{< /tab >}}

{{< /tabs >}}


## Recording Strategies
An application may use any mechanism it wants to start and stop recording
user audio. A couple of common methods are described below.

### Wake Word
In this strategy, the application uses a preselected word or short phrase that
a user must say to indicate they are talking to the computer (e.g.,
"Ok, Cobalt"). Often an application using this strategy will first send audio
to a dedicated wake word detector until the wake word is recognized in
the audio. At that point, audio is then routed to the ASR stream until
the stream returns a result. After the result is processed, audio is
once again sent to the wake word detector.

### Push-to-Talk
Systems using a push-to-talk strategy wait for a user to physically
push a button on a device or click a button in the UI to trigger
recording audio. There are two variants to this approach - one where
the user must hold the button for the entire time they are speaking,
and one where the user pushes the button once to start recording,
then once more to stop recording. The difference between the two
variants is mainly a question of preference. An application may choose
either approach.

Once the application detects that a button was pushed, it creates the ASR
stream and immediately begins sending audio to it. The application continues
to send audio to the stream until the button is released (or pressed
again), at which point the application should indicate that the audio source
has reached EOF if using the [convenience functions](#convenience-functions),
or call the [result method](#asr-result) if [sending audio](#sending-audio)
directly to the stream.

If Diatheke returns an ASR result before the user has released the button,
the application should discard additional audio (or temporarily stop recording)
until the next WaitForUser action is encountered while updating the session.
When it is time to record again, the application creates a new ASR stream.
