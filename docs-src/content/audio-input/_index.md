---
title: "Audio Input"
description: "Describes how to provide audio-based user input to a session."
weight: 600
---

To process a user's speech, the calling application may create an
[ASR](../glossary/#asr) stream. Bytes of audio data are sent to the
stream until the stream is closed and an ASR result, which includes
a transcript of the speech, is returned. The audio data encoding must
match what is expected by the server. The sample rate for any given
model is provided with the model information returned by the
[ListModels](../connecting/#list-models) method.

## Creating the ASR Stream

Use the session token to create the ASR stream.

Developers who are creating their own bindings for the
[gRPC interface](../protobuf) should note that the first message sent
on the stream returned by the `StreamASR` method should always be the
session token.

{{< tabs >}}

{{< tab "Go" >}}

```go
// Create the ASR stream. This automatically sends the session
// token first on the new stream.
stream, err := client.NewSessionASRStream(context.Background(), session.Token)
```

{{< /tab >}}

{{< tab "Python" }}

```python
# Create the ASR stream. This automatically sends the session
# token first on the new stream.
stream = client.new_session_asr_stream(session.token)
```

{{< /tab >}}

{{< tab "C++" "c++" >}}

```c++
// Create the ASR stream. This automatically sends the session
// token first on the new stream.
Diatheke::ASRStream stream = client.newSessionASRStream(session.token());
```

{{< /tab >}}

{{< tab "Swift/iOS" >}}

```swift
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
```

{{< /tab >}}

{{< tab "Java/Android">}}

```java
// Example coming soon!
```

{{< /tab >}}

{{< /tabs >}}

## Convenience Functions

The SDK includes some convenience functions to make it easier to send
audio data to Diatheke and get a result. These typically use a reader
type with the ASR stream created earlier. Applications are more than welcome
to instead use the stream methods directly to [send audio](#sending-audio)
and [get a result](#asr-result) if these convenience functions do not
meet their needs.

{{< tabs >}}

{{< tab "Go" >}}

```go
// Set up the audio source as an io.Reader
var audioSource io.Reader

// send 8kB chunks at a time
buffSize := 8192

// Process audio until we get a result or the audioSource returns
// io.EOF, whichever comes first.
result, err := diatheke.ReadASRAudio(stream, audioSource, buffSize)
fmt.Printf("transcript: %v\n", result.Text)
```

{{< /tab >}}

{{< tab "Python" >}}

```python
# The audio source should implement a read(size) function, such as
# a file object or process pipe.
audioSource = getAudioSource()

# send 8kB chunks at a time
buffSize = 8192

# Process audio until we get a result or the reader returns empty
# bytes (EOF), whichever comes first.
result = diatheke.read_ASR_audio(stream, audioSource, buffSize)
print("transcript:", result.text)
```

{{< /tab >}}

{{< tab "C++" >}}

```c++
// The audio source should inherit from the Diatheke::AudioReader class.
Diatheke::AudioReader* audioSource;

// Send 8kB chunks at a time
size_t buffSize = 8192;

// Process audio until we get a result or the number of bytes read
// (as returned by the Diatheke::AudioReader) is zero.
cobaltspeech::diatheke::ASRResult result =
    Diatheke::ReadASRAudio(stream, audioSource, buffSize);
std::cout << "transcript: " << result.text() << std::endl;
```

{{< /tab >}}

{{< /tabs >}}

## Sending Audio

After the stream is created, audio data is sent repeatedly over the
stream until the stream is closed or there is no more audio to send.

{{< tabs >}}

{{< tab "Go" >}}

```go
var audioData []byte
for haveAudioData {
	audioData = getFromSource()

	// Send the audio bytes
	if err := stream.SendAudio(audioData); err == io.EOF {
		// The stream has been closed, and a result is available.
		break
	}
}
```

{{< /tab >}}

{{< tab "Python">}}

```python
while haveAudioData:
    audioData = getFromSource()

    # Send the audio bytes
    if not stream.send_audio(audioData):
        # The stream has been closed, and a result is available
        break
```

{{< /tab >}}

{{< tab "C++" >}}

```c++
while (haveAudioData) {
    std::string audioData = getFromSource();

    // Send the audio bytes
    if (!stream.sendAudio(audioData)) {
        // The stream has been closed, and a result is available
        break;
    }
}
```

{{< /tab >}}

{{< tab "Swift/iOS" >}}

```swift
// Send the audio bytes
asrStream.sendAudio(data: data, completion: { (error) in
	if let error = error {
        	print(error.localizedDescription)
	}
})
```

{{< /tab >}}

{{< /tabs >}}

## ASR Result

When there is no more audio data to send, or if the ASR stream was
closed by the server, the application may retrieve the ASR result, which
includes a transcript of what was spoken. This result may be used to
[update a session](../sessions/update/#process-asr-result).

{{< tabs >}}

{{< tab "Go" >}}

```go
// Get the result from the stream. If the stream has not already
// been closed by the server, this will close it.
result, err := stream.Result()

// The transcript
fmt.Printf("transcript: %v\n", result.Text)

// The confidence the ASR system has in the given transcript,
// given as a value between 0 and 1.0, with 1.0 being the most
// confident in the result.
fmt.Printf("confidence: %v\n", result.Confidence)
```

{{< /tab >}}

{{< tab "Python">}}

```python
# Get the result from the stream. If the stream has not already
# been closed by the server, this will close it.
result = stream.result()

# The transcript
print("transcript:", result.text)

# The confidence the ASR system has in the given transcript,
# given as a value between 0 and 1.0, with 1.0 being the most
# confident in the result.
print("confidence:", result.confidence)
```

{{< /tab >}}

{{< tab "C++" >}}

```c++
// Get the result from the stream. If the stream has not already
// been closed by the server, this will close it.
auto result = stream.result();

// The transcript
std::cout << "transcript: " << result.text() << std::endl;

// The confidence the ASR system has in the given transcript,
// given as a value between 0 and 1.0, with 1.0 being the most
// confident in the result.
std::cout << "confidence: " << result.confidence() << std::endl;
```

{{< /tab >}}

{{< tab "Swift/iOS" >}}

```swift
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
```

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
