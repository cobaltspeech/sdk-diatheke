---
title: "Push Text"
description: "Provides text-based user input to a Diatheke session."
weight: 243
---

There are two ways to provide user input to a session:
[audio](../audio-input) and text. Text input is sent using the `PushText`
method as described below. Diatheke will respond with appropriate events
on the session's [event stream](../event-stream).

{{%tabs %}}

{{% tab "Go" %}}
``` go
// Push text using the client and sessionID
err := client.PushText(context.Background(), sessionID, "What's the capitol of Assyria?")

// OR push text using the session object
err := session.PushText(context.Background(), "What's the capitol of Assyria?")
```
{{% /tab %}}

{{% tab "C++" %}}
``` c++
// Push text using the client and sessionID
client.pushText(sessionID, "What's the capitol of Assyria?");

// OR push text using the session object
session.pushText("What's the capitol of Assyria?");
```
{{% /tab %}}

{{% /tabs %}}

{{% panel theme="info" header="Note" %}}
It is perfectly acceptable to use the `PushText` method while also
using audio input. The choice to do so is left as a design decision
for individual applications.
{{% /panel %}}