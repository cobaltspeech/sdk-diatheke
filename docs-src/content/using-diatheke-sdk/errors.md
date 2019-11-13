---
title: "Error Handling"
description: "Describes how errors from the SDK are reported."
weight: 22
---

For the sake of clarity, most examples in the documentation do not fully
demonstrate how to handle errors, preferring instead to focus on the topic
at hand. However, the Diatheke SDK does report errors, and client
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
