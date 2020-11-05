---
title: "Deleting a Session"
description: "How to end a session and clean up."
weight: 540
---

When the calling application is done with a session, it should always delete it.
This will help ensure there are no artifacts for the session left on the
Diatheke server. A session is "done" when the user is unlikely to be continuing
the same dialog; determining when that occurs is application-specific and may vary.
A task-oriented interface, for example, could reasonably end a session as soon
as a task is completed, assuming the next command should start from a fresh state.
On the other hand, an application in which users can ask follow-up questions
may keep the same session active for some time in order to retain context.
For example, if a user asks "When was Abraham Lincoln born?" and then a minute
after getting an answer asks "When did he die?", they would expect the second
question to continue to apply to Abraham Lincoln. But if they asked a new question
an hour later, they would probably not still be referring the same subject, so
should have a new session. In such a case, the calling application should delete
the session after an appropriate amount of time has passed with no further user input.

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
client.deleteSession(token).response.whenComplete { (result) in
	switch result {
	case .success(_):
		self.token = nil
	case .failure(let error):
		print(error.localizedDescription)
	}
}
{{< /tab >}}

{{< tab "Java/Android" "java" >}}
// Example coming soon!
{{< /tab >}}

{{< /tabs >}}
