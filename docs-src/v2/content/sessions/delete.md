---
title: "Deleting a Session"
description: "How to end a session and clean up."
weight: 540
---

When the SDK app is done with a session, it should always delete it. This
will help ensure there are no artifacts for the session left on the
Diatheke server. The criteria for when a session is "done", is application
specific and may vary. One good rule of thumb is that a session may be
considered done when a user has no intention of interacting with the
dialog further.

{{< tabs >}}

{{< tab "Go" "go" >}}
err = client.DeleteSession(context.Background(), session.Token)
{{< /tab >}}

{{< tab "Python" "python" >}}
client.delete_session(session.token)
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
