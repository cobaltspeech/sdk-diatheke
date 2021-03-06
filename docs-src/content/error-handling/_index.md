---
title: "Error Handling"
description: "Describes how errors from the SDK are reported."
weight: 300
---

For the sake of clarity, most examples in the documentation do not fully
demonstrate how to handle errors, preferring instead to focus on the topic
at hand. However, the Diatheke SDK does report errors, and calling
applications should be prepared to handle them.

A description of how errors are handled for each language is given below.

## C++

The C++ SDK uses [exceptions](http://www.cplusplus.com/doc/tutorial/exceptions/)
to report errors. Errors originating from the SDK will have the class
type [`Diatheke::ClientError`](https://github.com/cobaltspeech/sdk-diatheke/blob/master/grpc/cpp-diatheke/diatheke_client_error.h),
which inherits from the `std::exception` class.
To handle these errors, simply create a try-catch block around the SDK code.
For example:

``` c++
try
{
    // Call SDK functions in here. The function calls may be in this
    // try block, or within functions that the try block calls.
}
catch (const Diatheke::ClientError &err)
{
    // Handle the error here. The specific error message can be
    // retrieved using err.what().
}
```


## Go

The Go SDK uses the built-in `error` type to return errors from functions
(see [here](https://blog.golang.org/error-handling-and-go) for general
information about handling errors in Go). Most SDK functions will return
an error in addition to their other return values. For example:

``` go
// Create a new client connection
client, err := diatheke.NewClient("127.0.0.1:9002")
if err != nil {
    // Handle the error here.
}
```


## Python

The Python SDK uses [built-in exceptions](https://docs.python.org/3/library/exceptions.html)
to report errors. They can be handled by wrapping the client code in a
[try ... except statement](https://docs.python.org/3/tutorial/errors.html#handling-exceptions).
For example:

```python
try:
    # Call SDK functions in here. The SDK functions may be
    # called directly, or within other functions that this
    # block eventually calls.

except BaseException as err:
    # Handle the error here. To have better control over how
    # individual errors are handled, you may use more concrete
    # exception types in the except statement.

finally:
    # Do any final cleanup here.
```


## Swift

In the Swift SDK every method that can throw an error has
`DiathekeFailureCallback` completion handler to report errors. For example:

```swift
client.createSession(modelID: model.id) { (sessionOutput) in
    // Handle success response here
} failure: { (error) in
    // Handle the error here
    print(error.localizedDescription)
}
```
## Android

For synchronous calls, Java Exceptions are thrown by the generated gRPC calls, allowing the use of Try Catch blocks.

```java
try {
    SessionStart req = SessionStart.newBuilder().setModelId(modelId).build();
    SessionOutput resp = mDiathekeBlockingService.createSession(req);
} catch (StatusRuntimeException e) {
    // Handle errors here
}
```

For asynchronous calls, the StreamObservers will include an onError callback method.

```java
// Create the callback observer
StreamObserver<ASRResult> responseObserver = new StreamObserver<ASRResult>() {
    @Override
    public void onNext(ASRResult result) {
    }

    @Override
    public void onError(Throwable t) {
        // Handle errors here
    }

    @Override
    public void onCompleted() {
    }
};

// Create stream
StreamObserver<ASRInput> requestObserver = mDiathekeService.streamASR(responseObserver);

// Send data
requestObserver.onNext(cfg);
requestObserver.onNext(audio);
```
