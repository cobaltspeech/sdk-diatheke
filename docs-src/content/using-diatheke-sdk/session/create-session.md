---
title: "Creating a Session"
description: "How to start and end a new dialog using a Diatheke model to create a session."
weight: 241
---

New sessions may be created using a Diatheke model, which defines what
is allowed in the session's conversation. For example, if the model
defines an [intent](../../../glossary#intent) that includes the phrase
"check account balance", a user may speak that phrase and expect
Diatheke to respond. If, however, the user speaks the phrase "buy pizza",
and that intent is *not* in the model, Diatheke may ignore the phrase or
reply with an error message, such as "I didn't understand that".

To begin a new dialog, create a session by doing the following:

{{%tabs %}}

{{% tab "Go" %}}
```go
// The Diatheke model ID is defined in the Diatheke server config file.
diathekeModelID := "1"
sessionID, err := client.NewSession(context.Background(), diathekeModelID)

// Be sure to close the session when we are done with it.
defer client.EndSession(context.Background(), sessionID)
```
{{% /tab %}}

{{% tab "C++" %}}
```c++
// The Diatheke model ID is defined in the Diatheke server config file.
std::string diathekeModelID = "1";
std::string sessionID = client.newSession(diathekeModelID);

// Be sure to close the session when we are done with it.
client.endSession(sessionID);
```
{{% /tab %}}

{{% tab "Python" %}}
``` python
# The Diatheke model ID is defined in the Diatheke server config file.
diatheke_model_id = "1"
session_id = client.new_session(diatheke_model_id)

# Be sure to close the session when we are done with it.
client.end_session(session_id)
```
{{% /tab %}}

{{% /tabs %}}

Creating a new session will return a session ID, which is required for all
the session functions in the SDK.

As shown in the example, sessions should be closed when they are no longer
in use. This has the side-effect of freeing resources related to the session
on the server.

It should also be noted that simply exiting the application or closing
the client will not end a session on the server. This is by design - in
some situations, it may be convenient to save the sessionID and resume
session interactions after creating a new client connection. For example,
it might not make sense to preserve the network connection for a long
running [commands](../../../glossary#command). The client code could save
the session ID, close the client connection, execute the command, then
restart the client and resume the session when the command has finished.


## The Session Object
For those who prefer a more object oriented approach to interacting with
sessions, the SDK provides a convenience class that wraps the client and
session ID. The session object includes all the session functions as class
members.

{{%tabs %}}

{{% tab "Go" %}}
``` go
// Wrap the session ID in the convenience class
session := diatheke.Session{
    ID: sessionID,
    Parent: client,
}

// Use session methods without the ID as a parameter
err := session.EndSession(context.Background());
```
{{% /tab %}}

{{% tab "C++" %}}
``` c++
// Wrap the session ID in the convenience class
Diatheke::Session session(sessionID, &client);

// Use session methods without the ID as a parameter
session.EndSession();
```
{{% /tab %}}

{{% tab "Python" %}}
``` python
# Wrap the session ID in the convenience class
session = diatheke.Session(session_id, client)

# Use session methods without the ID as a parameter
session.end_session()
```
{{% /tab %}}

{{% /tabs %}}
