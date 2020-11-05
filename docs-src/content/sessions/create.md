---
title: "Creating a Session"
description: "How to create a new session."
weight: 510
---

To create a new session, the calling application must call the
`CreateSession` method with the desired model ID as specified in the
server configuration. To get a list of available models, see
[here](../../connecting/#models). A new session token will be returned
along with a list of [actions](../actions).

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
// The specific model ID is defined by the Diatheke server.
let modelID = "someID"
client.createSession(modelID: modelID) { (sessionOutput) in
	// Access the session token
	self.token = sessionOutput.token
	// Access the action list
	self.actionList = sessionOutput.actionList
} failure: { (error) in
	print(error.localizedDescription)
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}


A new session always starts at the same place in the Diatheke model, with
nothing saved in memory, which makes it a convenient way to reset the
dialog for a user when necessary. 

Typically, each user has a single session. However, depending
on it's structure and goals, it is possible for an application to create
multiple sessions (perhaps with different models) for a single user, which
gives the user the flexibility to put one dialog on hold while responding
to a different dialog, with each dialog state being stored in their
respective sessions.

An application is allowed to cache the session token for as long as they
like. When it is used again, the session will continue from the state
represented by the token.
