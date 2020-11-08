---
title: "Creating a Session"
description: "How to create a new session."
weight: 510
---

To create a new session, the SDK app must call the `CreateSession` method
with the desired model ID. To get a list of available models, see
[here](../../connecting/#models). A new session token will be returned
with a list of [actions](../actions).

{{< tabs >}}

{{< tab "Go" "go" >}}
// The specific model ID is defined by the Diatheke server.
modelID := "someID"
session, err := client.CreateSession(context.Background(), modelID)

// Access the session token
token := session.Token

// Access the action list
actionList := session.ActionList
{{< /tab >}}

{{< tab "Python" "python" >}}
# The specific model ID is defined by the Diatheke server.
model_id = "someID"
session = client.create_session(model_id)

# Access the session token
token = session.token

# Access the action list
action_list = session.action_list
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


A new session always starts at the same place in the Diatheke model, with
nothing saved in memory, which makes it a convenient way to reset the
dialog for a user when necessary. 

Typically, a single session is used by a single user. However, depending
the app's structure and goals, it is possible for an app to create
multiple sessions (perhaps with different models) for a single user, which
gives the user the flexibility to put one dialog on hold while responding
to a different dialog, with each dialog state being stored in their
respective sessions.
