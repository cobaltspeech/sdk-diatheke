# SDK for Diatheke (Cobalt's Dialog Management Engine)

This repository contains the SDK for Cobalt's Diathele Dialog Management Engine.

This README has instructions to _build_ the SDK.  For installing and using the
SDK, see the [SDK Docs](https://cobaltspeech.github.io/sdk-diatheke).

## Network API (using GRPC)

The `grpc` folder at the top level of this repository contains code for Diatheke's
GRPC API.  The `grpc/diatheke.proto` file is the authoritative service definition of
the API and is used for auto generating SDK code in multiple languages.

### Auto-generated code and documentation

The `grpc` folder contains auto-generated code in several languages.  In order
to generate the code again, you should run `make`.  Generated code is checked
in, and you must make sure it is up to date when you push commits to this
repository.

Code generation has the following dependencies:
  - The protobuf compiler itself (protoc)
  - The protobuf documentation generation plugin (protoc-gen-doc)
  - The golang plugins (protoc-gen-go and protoc-gen-grpc-gateway)
  - The python plugins (grpcio-tools and googleapis-common-protos)
  - The swift plugins (protoc-gen-swift and protoc-gen-grpc-swift)
  - The static website generator (hugo)

A few system dependencies are required:
  - Go >= 1.14
  - git
  - python3
  - virtualenv
  - unzip
  - wget
  - swift >= 5.0.1

The top level Makefile can set up all other dependencies.

Documentation is authored in the `docs-src` folder and generated static website
is stored in the `docs` folder.

To generate the code and documentation, run `make`.  This is currently only
supported under linux.

If you are doing local development on the docs, you can use this command to
serve it locally:

```
cd docs-src
../deps/bin/hugo server -D
```

### Tagging New Versions

This repository has several components, and they need more than just a "vX.Y.Z"
tag on the git repo.  In particular, this repository has two go modules, one of
which depends on the other, and in order to make sure correct versions are used,
we need to follow a few careful steps to release new versions on this
repository.

Step 1: Make sure all generated code and documentation is up to date.

```
pushd grpc && make && popd
pushd docs-src && hugo -d ../docs && popd
git diff --quiet || echo "You have uncommitted changes.  Please get them merged in via a PR before updating versions."
```

Step 2: Update the version number.

In addition to the git tags, we also save the version string in a few places in
our sources.  These strings should all be updated and a new commit created.  The
git tags should then be placed on that commit once merged to master.

Decide which version you'd like to tag. For this README, let's say the next
version to tag is `1.0.1`.

Step 3: Add version tags to the sources.

```
NEW_VERSION="1.0.1"

git checkout master
git checkout -b version-update-v$NEW_VERSION

sed -i 's|DiathekeSDKVersion = "[0-9.]*"|DiathekeSDKVersion = "'$NEW_VERSION'"|g' grpc/swift-diatheke/Diatheke.swift
sed -i 's|.upToNextMajor(from: "[0-9.]*")|.upToNextMajor(from: "'$NEW_VERSION'")|g' docs-src/content/using-diatheke-sdk/include.md
pushd docs-src && hugo -d ../docs && popd

git commit -m "Update version to v$NEW_VERSION"
git push origin version-update-v$NEW_VERSION
```

Step 4: Create a pull request and get changes merged to master.

Step 5: Create version tags on the latest master branch:

```
git checkout master
git pull origin master
git tag -a v$NEW_VERSION -m ''
git tag -a grpc/go-diatheke/v$NEW_VERSION -m ''
git push origin --tags
```
