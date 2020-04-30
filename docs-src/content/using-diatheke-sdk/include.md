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


## Python
The Python SDK requires Python v3.5.0 or greater. The SDK may be installed
from GitHub using `pip`:

```bash
pip install "git+https://github.com/cobaltspeech/sdk-diatheke#egg=cobalt-diatheke&subdirectory=grpc/py-diatheke"
```

Once installed, simply import the Diatheke module to use the SDK:

```python
import diatheke
```

### iOS

#### Swift Package Manager

The [Swift Package Manager](https://swift.org/package-manager/) is a tool for automating the distribution of Swift code and is integrated into the `swift` compiler.

Once you have your Swift package set up, adding swift-cubic as a dependency is as easy as adding it to the `dependencies` value of your `Package.swift`.

```swift
dependencies: [
    .package(url: "https://github.com/cobaltspeech/sdk-diatheke.git", .upToNextMajor(from: "1.0"))
]
```
