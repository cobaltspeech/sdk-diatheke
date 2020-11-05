---
title: "Event Stream"
description: "The event stream notifies the client of significant events that occur in Diatheke."
weight: 242
---

Diatheke server uses an 
[event driven architecure](https://en.wikipedia.org/wiki/Event-driven_architecture)
to notify clients when significant events happen for a session. These
include [Recognize](#recognize-event), [Reply](#reply-event), and
[Command](#command-event) events. These events are delivered to the
client via a session's event stream.


## Creating the Event Stream
The following demonstrates how to create a an event stream for a session.
{{< tabs >}}

{{< tab "Go" "go" >}}
// Create the stream using the client and session ID.
stream, err := client.SessionEventStream(context.Background(), sessionID)

// OR create the stream using the Session object
stream, err := session.EventStream(context.Background())
{{< /tab >}}

{{< tab "C++" "c++" >}}
std::unique_ptr<Diatheke::EventStream> stream;

// Create the stream using the client and session ID
stream = client.sessionEventStream(sessionID);

// OR create the stream using the session object
stream = session.eventStream();
{{< /tab >}}

{{< tab "Python" "python">}}
# Create the stream using the client and session ID
stream = client.session_event_stream(session_id)

# OR create the stream using the Session object
stream = session.event_stream()
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Create the stream using the client and session ID
let stream = client.sessionEventStream(sessionID: sessionID) { (event) in
    // Handle the event
}

// OR create the stream using the Session object
let stream = session.eventStream { (event) in
    // Handle the event     
}
{{< /tab >}}

{{< /tabs >}}

This creates a server stream that will deliver events to the client. The
stream will be closed by the server when the session ends.


## Event Handling
The recommended way to handle events from the stream is to setup a loop
that waits for the next event, then handles the event based on its type:

{{< tabs >}}

{{< tab "Go" "go" >}}
for {
    // Wait for the next event from the server
    event, err := eventStream.Recv()

    // This indicates the stream has finished, which will happen
    // when the session ends or the context used to create the stream
    // closes.
    if err == io.EOF {
        break
    }

    // Handle any other errors
    if err != nil {
        fmt.Printf("Error: %v\n", err)
        break
    }

    // Handle the event based on its type.
    switch e := event.Result.(type) {
    case *diathekepb.DiathekeEvent_Recognize:
        handleRecognizeEvent(e.Recognize)

    case *diathekepb.DiathekeEvent_Reply:
        handleReplyEvent(e.Reply)

    case *diathekepb.DiathekeEvent_Command:
        handleCommandEvent(e.Command, manager.Session)

    default:
        fmt.Printf("Error: received unknown event type from Diatheke\n")
    }
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Receive events from the event stream until it is closed, which will
// happen when the session ends, or the context used to create the stream
// closes.
cobaltspeech::diatheke::DiathekeEvent event;
while (eventStream->waitForEvent(&event))
{
    // Check for the event type
    if (event.has_recognize())
    {
        handleRecognizeEvent(event.recognize());
    }
    else if (event.has_reply())
    {
        handleReplyEvent(event.reply());
    }
    else if (event.has_command())
    {
        handleCommandEvent(event.command(), eventStream.get());
    }
    else
    {
        std::cerr << "Received unknown event type from Diatheke" << std::endl;
    }
}

// It is good practice to close the event stream when we are done with it.
// This will allow the server to report errors related to the stream, if 
// there were any.
eventStream->close();
{{< /tab >}}

{{< tab "Python" "python">}}
# Receive events from the event stream until it is closed, which will
# happen when the session ends.
for event in event_stream:
    # Check the event type
    if event.HasField("recognize"):
        handle_recognize_event(event.recognize)

    elif event.HasField("reply"):
        handle_reply_event(event.reply)

    elif event.HasField("command"):
        handle_command_event(event.command, session)

    else:
        print("Received unknown event type from Diatheke.")
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Receive events from the event stream until it is closed, which will
// happen when the session ends.
let stream = client.sessionEventStream(sessionID: sessionID) { (event) in
    guard let result = event.result else { return }

    // Check the event type
    switch result {
        case .recognize(let recognizeEvent):
            self.handleRecognizeEvent(recognizeEvent)
        case .reply(let replyEvent):
            self.handleReplyEvent(replyEvent)
        case .command(let commandEvent):
            self.handleCommandEvent(commandEvent, session: session)
    }
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
mClient.sessionEventStream(session, new StreamObserver<DiathekeOuterClass.DiathekeEvent>() {
    @Override
    public void onNext(DiathekeOuterClass.DiathekeEvent value) {
        switch (value.getResultCase()) {
                case COMMAND:
                    DiathekeOuterClass.CommandEvent command = value.getCommand();
                    break;
                case RECOGNIZE:
                    DiathekeOuterClass.RecognizeEvent recognize = value.getRecognize();
                    break;
                case REPLY:
                    DiathekeOuterClass.ReplyEvent reply = value.getReply();
                    break;
                default:
                    break;
            }
        }
    }

    @Override
    public void onError(Throwable t) {

    }

    @Override
    public void onCompleted() {

    }
});
{{< /tab >}}
{{< /tabs >}}


### Recognize Event
The Recognize event is sent when Diatheke has received user input. In
the case of [text-based](../push-text) input, the text of the event
will be the same as what was typed by the user. In the case of
[audio-based](../audio-input) input, the event is sent when enough audio
has been processed that a transcription is available. In this case, the
text of the event will be the ASR transcription. The event will also
indicate whether the user input was recognized by the
[NLU](../../../glossary#nlu) as a valid 
[intent](../../../glossary#intent) or [entity](../../../glossary#entity).

Most often, this event will be used by clients to give some kind of
visual indication to the user about whether their input was recognized
or not.

{{< tabs >}}

{{< tab "Go" "go" >}}
func handleRecognizeEvent(event *diathekepb.RecognizeEvent) {
    // Check if Diatheke recognized the last input as valid.
    if event.ValidInput {
        fmt.Printf("Valid input: %s\n", event.Text)
    } else {
        fmt.Printf("Invalid input: %s\n", event.Text)
    }
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
void handleRecognizeEvent(const cobaltspeech::diatheke::RecognizeEvent &event)
{
    if(event.valid_input())
    {
        std::cout << "Valid input: " << event.text() << std::endl;
    }
    else
    {
        std::cout << "Invalid input: " << event.text() << std::endl;
    }
}
{{< /tab >}}

{{< tab "Python" "python">}}
def handle_recognize_event(event):
    if event.valid_input:
        print("Valid input: " + event.text)
    else:
        print("Invalid input: " + event.text)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func handleRecognizeEvent(_ event: Cobaltspeech_Diatheke_RecognizeEvent) {
    // Check if Diatheke recognized the last input as valid.
    if event.validInput {
        print("Valid input: \(event.text)")
    } else {
        print("Invalid input: \(event.text)")
    }
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void handleRecognizeEvent(DiathekeOuterClass.RecognizeEvent event) {
    // Check if Diatheke recognized the last input as valid.
    if (event.getValidInput()) {
        Log.i("RecognizeEvent",String.format("Valid input: %s\n", event.getText()));
    } else {
        Log.i("RecognizeEvent",String.format("Invalid input: %s\n",event.getText()));
    }
}

{{< /tab >}}

{{< /tabs >}}


### Reply Event
The Reply event is sent when, based on the session's model, Diatheke has a
reply for the user. This might happen when Diatheke needs additional
information from the user, in which case the text of the Reply event will
be a prompt. This event may also be sent to give the user information,
such as after running a command. The text for all Reply events is
defined in the Diatheke model.

Most often, this event will be used by clients to give some kind of visual
feedback to the user.

{{< tabs >}}

{{< tab "Go" "go" >}}
func handleReplyEvent(event *diathekepb.ReplyEvent) {
    fmt.Printf("Reply text: %s\n", event.Text)
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
void handleReplyEvent(const cobaltspeech::diatheke::ReplyEvent &event)
{
    std::cout << "Reply text: " << event.text() << std::endl;
}
{{< /tab >}}

{{< tab "Python" "python">}}
def handle_reply_event(event):
    print("Reply text: " + event.text)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func handleReplyEvent(_ event: Cobaltspeech_Diatheke_ReplyEvent) {
    print("Reply text: \(event.text)")
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void handleReplyEvent(DiathekeOuterClass.ReplyEvent reply) {
    Log.i("ReplyEvent",reply.getText());
}
{{< /tab >}}

{{< /tabs >}}


### Command Event
The Command event is sent when Diatheke wants the client to execute a
command. This happens after the corresponding
[intent](../../../glossary#intent) and [entities](../../../glossary#entity)
have been recognized, and Diatheke is in a state where the command has
all the required information to execute (as defined by the Diatheke model).

From the client perspective, this is the most important event that will
come from Diatheke because it is what prompts the client code to do useful
work by executing the commands.

{{< tabs >}}

{{< tab "Go" "go" >}}
func handleCommandEvent(event *diathekepb.CommandEvent, session diatheke.Session) {
    // Use the command ID and parameters to execute a task.
    fmt.Printf("Command ID: %v\n", event.CommandId)
    fmt.Printf("Parameters:\n")
    for param, value := range event.Parameters {
        fmt.Printf("  %v = %v\n", param, value)
    }
    fmt.Printf("\n")

    // After executing the command, be sure to notify Diatheke that
    // we are done. This is important to do so that dialog flow may continue
    // after the command is finished.
    status := &diathekepb.CommandStatus{
        SessionId: session.ID,
        CommandId: event.CommandId,

        // Indicate success or failure.
        ReturnStatus: diathekepb.CommandStatus_SUCCESS,

        // If the ReturnStatus above is CommandStatus_FAILURE,
        // the ErrorMessageText field of this struct should also be populated.
        // ErrorMessageText: "some message describing the error",

        // Return parameters as necessary. Depending on the Diatheke model,
        // some commands may be expected to have output.
        OutputParameters: event.Parameters,

        // Internal data that should always be set from the original command
        // event.
        CommandStateId: event.CommandStateId,
    }
    
    if err := session.CommandFinished(context.Background(), status); err != nil {
        fmt.Printf("Error: %v\n", err)
    }
}
{{< /tab >}}

{{< tab "C++" "c++" >}}
void handleCommandEvent(const cobaltspeech::diatheke::CommandEvent &event,
                        Diatheke::EventStream *stream)
{
    // Use the command ID and parameters to execute a task.
    std::cout << "Command ID: " << event.command_id() << std::endl;
    std::cout << "Parameters:" << std::endl;
    for (const auto &pair : event.parameters())
    {
        std::string param = pair.first;
        std::string value = pair.second;
        std::cout << "  " << param << " " << value << std::endl;
    }
    std::cout << std::endl;

    Diatheke::CommandStatus status(event);

    // Set the status code to indicate whether the command failed
    status.setStatusCode(Diatheke::CommandStatus::SUCCESS);

    // If the status code is FAILURE, the error message should also be set
    // status.setError("some message describing the error");

    // Update return parameters as necessary. For example:
    // status.setStringParam("some key", "some value");

    // Notify Diatheke that the command is finished. This is important
    // to do so that dialog flow may continue after the command is
    // finished.
    eventStream->commandFinished(status);
}
{{< /tab >}}

{{< tab "Python" "python">}}
def handle_command_event(event, session):
    # Use the command ID and parameters to execute a task.
    print("Command ID: " + event.command_id)
    print("Parameters:")
    for param in event.parameters:
        value = event.parameters[param]
        print("  {} = {}".format(param, value))
    print("\n")

    # After executing the command, be sure to notify Diatheke that we
    # are done. This is important to do so that dialog flow may continue
    # after the command is finished.
    result = diatheke.CommandStatus()
    result.session_id = session.id
    result.command_id = event.command_id

    # Indicate success or failure
    result.return_status = diatheke.CommandStatus.SUCCESS

    # If the return_status above is diatheke.CommandStatus.FAILURE,
    # the error_message_text field of this object should also be
    # populated.
    # result.error_message_text = "some message describing the error"

    # Return parameters as necessary. Depending on the Diatheke model,
    # some commands may be expected to have output.
    result.output_parameters.update(event.parameters)

    # Internal data that should always be set from the original command
    # event.
    result.command_state_id = event.command_state_id

    # Send the result to Diatheke
    session.command_finished(result)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
func handleCommandEvent(_ event: Cobaltspeech_Diatheke_CommandEvent, session: Session) {
    // Use the command ID and parameters to execute a task.
    print("Command ID: \(event.commandID)")
    print("Parameters:")
    
    for (param, value) in event.parameters {
        print("  \(param) = \(value)")
    }
    
    let status = CommandStatus(command: event)
    
    // Set the status code to indicate whether the command failed
    status.statusCode = .success
    
    // If the status code is FAILURE, the error message should also be set
    status.errorMesage = "some message describing the error"

    // Update return parameters as necessary. For example:
    status.setParam(key: "some key", value: "some value")

    // Notify Diatheke that the command is finished. This is important
    // to do so that dialog flow may continue after the command is
    // finished.
    session.commandFinished(commandStatus: status) { (error) in
        print(error.localizedDescription)
    }
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
private void handleCommandEvent(DiathekeOuterClass.CommandEvent command) {
    Log.i("CommandEvent",command.getCommandId());
    Log.i("CommandEvent",command.getCommandStateId());
    Map<String, String> parametersMap = command.getParametersMap();
    for (Map.Entry<String, String> entity : parametersMap.entrySet()) {
        Log.i("CommandEvent",String.format("%s=%s",entity.getKey(),entity.getValue()));
    }
}
{{< /tab >}}

{{< /tabs >}}


#### Command Finished
After receiving a Command event, it is important to notify Diatheke when
command execution has completed. Not only does it let Diatheke know the
final command status (success or failure), but calling `CommandFinished`
allows Diatheke to continue processing and execute any additional commands
or replies as defined by the Diatheke model.

For long running commands, it should be noted that calling `CommandFinished`
will restore the dialog to the state Diatheke was at when the Command event
was sent. This allows dialog to remain responsive to other intents while
waiting for long-running commands to finish.

{{< tabs >}}

{{< tab "Go" "go" >}}
// Use the client
err := client.CommandFinished(context.Background(), status)

// OR use the Session object
err := session.CommandFinished(context.Background(), status)
{{< /tab >}}

{{< tab "C++" "c++" >}}
// Use the client
client.commandFinished(sessionID, status);

// OR use the Session object
session.commandFinished(status);

// OR use the EventStream object
stream->commandFinished(status);
{{< /tab >}}

{{< tab "Python" "python">}}
# Use the client
client.command_finished(status)

# OR use the Session object
session.command_finished(status)
{{< /tab >}}

{{< tab "Swift/iOS" "swift" >}}
// Use the client
client.commandFinished(sessionID: sessionID, commandStatus: status)

// OR use the Session object
session.commandFinished(commandStatus: status)
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
DiathekeOuterClass.CommandStatus status = DiathekeOuterClass.CommandStatus.newBuilder().build();
mClient.commandFinished(status, new StreamObserver<DiathekeOuterClass.Empty>() {
    @Override
    public void onNext(DiathekeOuterClass.Empty value) {
        
    }

    @Override
    public void onError(Throwable t) {

    }

    @Override
    public void onCompleted() {

    }
});

{{< /tab >}}

{{< /tabs >}}


## Event flow
### Audio Based
For an [audio-based](../audio-input) dialog, the flow events is shown below.
Based on the Diatheke model, the server will send events in response to
recognized audio and intents.

{{<mermaid align="center">}}
sequenceDiagram;
    Note left of Client: Client records<br>audio
    Client ->> Diatheke: Audio input
    Client ->> Diatheke: Audio input
    Client ->> Diatheke: Audio input
    Note right of Diatheke: ASR creates</br>transcript
    Note right of Diatheke: NLU interprets</br>transcript
    Diatheke -->> Client: Recognize event
    Note right of Diatheke: Model finds</br>command for intent
    Diatheke -->> Client: Command event
    Note left of Client: Client executes<br>command
    Client ->> Diatheke: CommandFinished
    Note right of Diatheke: Model finds</br>Say action</br>after command
    Diatheke -->> Client: Reply event
{{< /mermaid >}}

### Text Based
For a [text-based](../push-text) dialog, the flow events is shown below.
As with the audio-based dialog, the specific type and number of events
will depend on the Diatheke model.
{{<mermaid align="center">}}
sequenceDiagram;
    Note left of Client: Client sends<br>text
    Client ->> Diatheke: Text input
    Note right of Diatheke: NLU interprets</br>text
    Diatheke -->> Client: Recognize event
    Note right of Diatheke: Model finds</br>command for intent
    Diatheke -->> Client: Command event
    Note left of Client: Client executes<br>command
    Client ->> Diatheke: CommandFinished
    Note right of Diatheke: Model finds</br>another command
    Diatheke -->> Client: Command event
    Note left of Client: Client executes<br>command
    Client ->> Diatheke: CommandFinished
    Note right of Diatheke: Model finds</br>Say action</br>after command
    Diatheke -->> Client: Reply event
{{< /mermaid >}}
