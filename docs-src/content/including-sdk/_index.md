---
title: "Including the SDK"
description: "Gives language-specific instructions about how to add the SDK to your project."
weight: 200
---

Language-specific instructions to include the SDK in your project
are given below.

<!--more-->

## C++

To use the Diatheke SDK in a C++ project, you must first
download the SDK from Github.

```bash
git clone git@github.com/cobaltspeech/sdk-diatheke
```

To simplify the build process, the C++ SDK uses [CMake](http://www.cmake.org),
although it is possible to use a different build system with some additional
work. Details for building and including the SDK in a C++ project are
described in detail in [this README file](https://github.com/cobaltspeech/sdk-diatheke/blob/master/grpc/cpp-diatheke/README.md).


## Go
The Go SDK supports Go modules and requires Go 1.14 or later. To use the
SDK, import this package into your application. For v2 of the SDK and later,
be sure to include the version prefix in the import path (e.g., v2, v3, etc.).

``` go
import "github.com/cobaltspeech/sdk-diatheke/grpc/go-diatheke/v2"
```

{{% panel theme="info" header="Note" %}}
When using Go, it is not necessary to download the SDK using git.
The `go build` command will automatically fetch the sdk-diatheke code
from GitHub to use in your Go project.
{{% /panel %}}


## Python
The Python SDK requires Python v3.5.0 or greater. The SDK may be installed
from GitHub using `pip` as shown below, where `<VERSION>` is the specific
version of the SDK the application is targeting (e.g., `v1.3.0`, `v2.1.0`, etc.)

```bash
# Replace <VERSION> with the desired SDK version
pip install "git+https://github.com/cobaltspeech/sdk-diatheke@<VERSION>#egg=cobalt-diatheke&subdirectory=grpc/py-diatheke"
```

To get `v1.3.0`
```bash
pip install "git+https://github.com/cobaltspeech/sdk-diatheke@v1.3.0#egg=cobalt-diatheke&subdirectory=grpc/py-diatheke"
```

To get `v2.1.0`
```bash
pip install "git+https://github.com/cobaltspeech/sdk-diatheke@v2.1.0#egg=cobalt-diatheke&subdirectory=grpc/py-diatheke"
```

To get the latest master (not recommended for production)
```bash
pip install "git+https://github.com/cobaltspeech/sdk-diatheke#egg=cobalt-diatheke&subdirectory=grpc/py-diatheke"
```

Once installed, simply import the Diatheke module to use the SDK:

```python
import diatheke
```

## iOS

### Swift Package Manager

The [Swift Package Manager](https://swift.org/package-manager/) is a tool
for automating the distribution of Swift code and is integrated into the
`swift` compiler.

Once you have your Swift package set up, adding swift-cubic as a
dependency is as easy as adding it to the `dependencies` value of your
`Package.swift`.

```swift
dependencies: [
    .package(url: "git@github.com:cobaltspeech/sdk-diatheke.git", .upToNextMajor(from: "2.1.2"))
]
```

## Android (Java)

Support for Android/Java is provided via our official Maven package hosted on GitHub Packages.
We provide two versions of the Java library, a full version and a lite version.
Android users should use the lite version with its size and speed optimiaztions designed for mobile applications.
All other Java users are encouraged to use the full version, which has better compatibility and more tooling.

This documentation assumes the use of Gradle for the build process.
Integrating into the Gradle build process is a two step process.
First you need to add a definition for Cobalt's GitHub Maven Repository.
Once this is done, adding SDK-Diatheke as a dependency is straight forward.

### Adding Cobalt's GitHub Maven Repository

In depth documentation for this step can be found in [GitHub's documentation](https://docs.github.com/en/free-pro-team@latest/packages/using-github-packages-with-your-projects-ecosystem/configuring-apache-maven-for-use-with-github-packages#installing-a-package).

#### Authenticating

GitHub requires that you authenticate with them, even for pulling a public package.
You must create a Personal Access Token from GitHub via https://github.com/settings/tokens.
The generated token only needs access to `read:packages`.

Note: tokens should be kept private.  We suggest storing it in `github.properties` that is then added to your `.gitignore` file.
This file should have your username and token like this:

```properties
GITHUB_USER="<Your_GitHub_Username>"                     # Such as `cobaltspeech`
GITHUB_TOKEN="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" # Token generated from https://github.com/settings/tokens
```

#### Defining the Maven Repository

You can now point Gradle to our Maven repository with the credentials defined above.
You will need to add that definition to the top level `build.gradle` file, typically under the `allprojects.repositories` section.
The following will load your username and PAT from the `github.properties` file and then define the url and credentials to reach the Maven repository containing the SDK-Diatheke package.

```groovy
// File: /build.gradle
buildscript { ... }
allprojects {
    repositories {
        // These two are usually already in your project.
        google()  // Adds https://dl.google.com/dl/android/maven2/
        jcenter() // Adds https://jcenter.bintray.com/

        ////////// START //////////
        // First we load the username and access token from the github.properties file
        def githubPropertiesFile = rootProject.file("github.properties")
        def githubProperties = new Properties()
        githubProperties.load(new FileInputStream(githubPropertiesFile))

        // Now we define the new maven repository.
        maven {
            url = uri("https://maven.pkg.github.com/cobaltspeech/sdk-diatheke")
            credentials {
               username = githubProperties['GITHUB_USER']  // Your github username
               password = githubProperties['GITHUB_TOKEN'] // Personal Access Token created from https://github.com/settings/tokens
            }
        }
        ////////// END //////////
    }
}
```

### Importing the SDK

Now we need to pull from the package from that defined Maven Repository.
In your `app/build.gradle`, you will need to add the new dependencies.

```groovy
// File: /app/build.gradle
plugins {...}
android {...}
dependencies {
    ... // Your other pre-existing dependencies.

    // Here we add the sdk-diatheke client library.
    implementation('com.cobaltspeech.diatheke:sdk-diatheke-lite:2.0.0-rc1') {
        // Note: we exclude the protobuf-javalite here because we import it below.
        exclude group: 'com.google.protobuf', module: 'protobuf-javalite'
    }
    // These dependencies allow your application to use gRPC over HTTP.
    implementation 'com.google.protobuf:protobuf-javalite:3.13.0'
    implementation 'com.squareup.okhttp:okhttp:2.7.5'
    implementation 'io.grpc:grpc-core:1.33.0'
    implementation 'io.grpc:grpc-stub:1.33.0'
    implementation 'io.grpc:grpc-okhttp:1.33.0'
    // end sdk-diatheke client library/dependencies
}
```
