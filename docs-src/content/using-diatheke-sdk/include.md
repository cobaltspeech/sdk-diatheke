---
title: "Including the SDK"
description: "Gives language-specific instructions about how to add the SDK to your project."
weight: 21
---

Language-specific instructions to include the SDK in your project
are given below.

<!--more-->

## C++

To use the Diatheke SDK in a C++ project, you must first 
[download the SDK](../../getting-started#installing-the-diatheke-sdk).
To help simplify the build process, the C++ SDK uses
[CMake](http://www.cmake.org), although it is possible to use a different
build system with some additional work. Details for building and including
the SDK in a C++ project are described in detail in
[this README file](https://github.com/cobaltspeech/sdk-diatheke/blob/master/grpc/cpp-diatheke/README.md).


## Go
The Go SDK supports Go modules and requires Go 1.12 or later. To use the
SDK, import this package into your application:

``` go
import "github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke"
```

{{% panel theme="info" header="Note" %}}
When using Go, it is not necessary to download the SDK using git
(unless you want to build the example code).
The `go build` command will automatically fetch the sdk-diatheke code
from GitHub to use in your Go project.
{{% /panel %}}

