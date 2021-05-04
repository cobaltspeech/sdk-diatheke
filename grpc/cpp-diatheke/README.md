# Diatheke C++ SDK
Unlike other languages, C++ has more strict requirements to
integrate with gRPC. The easiest way to accomodate these
requirements is to build gRPC as part of the C++ project
and use it to generate the required C++ files from the 
diatheke.proto file.

## CMake Build
To help simplify the build process, this project uses
[CMake](www.cmake.org). CMake will automatically download
the gRPC source code and include it as a subproject, giving
access to the gRPC libraries and the protoc compiler.

To build as a standalone library execute the following
commands:
```bash
# Create a build directory. It can be named anything and
# can exist outside of the source code directory.
mkdir build-diatheke-client && cd build-diatheke-client

# Run CMake to download gRPC and generate makefiles.
# The final path specifies the directory that contains the
# CMakeLists.txt file for the diatheke_client C++ project. By
# default this will create static libraries. To make a shared
# library instead, add -DBUILD_SHARED_LIBS=TRUE
cmake -DCMAKE_BUILD_TYPE=Release <path/to/sdk-diatheke/grpc/cpp-diatheke>
# OR
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=TRUE <path/to/sdk-diatheke/grpc/cpp-diatheke>

# Build the library
make diatheke_client
```

To include this CMake project in another one, simply
copy this repository into your project and add the line

```cmake
add_subdirectory(sdk-diatheke/grpc/cpp-diatheke)
```

to your project's CMakeLists.txt. You may also include it
using the [FetchContent module](https://cmake.org/cmake/help/latest/module/FetchContent.html),
which has the added convenience of downloading a specific
version of the SDK from Github automatically.

```cmake
# Fetch Diatheke SDK code and add to the project
include(FetchContent)
FetchContent_Declare(
    sdk_diatheke
    GIT_REPOSITORY https://github.com/cobaltspeech/sdk-diatheke.git
    GIT_TAG v2.0.1
)
FetchContent_Populate(sdk_diatheke)

# The SDK's CMake file is not at the top-level directory of the repo,
# so we must tell CMake explicitly which subdirectory to add.
add_subdirectory(${sdk_diatheke_SOURCE_DIR}/grpc/cpp-diatheke ${sdk_diatheke_BINARY_DIR})
```

### Windows Builds
To build the C++ SDK on Windows, we strongly recommend using
CMake. As this CMake project will also build the gRPC library,
be sure that you have all the
[gRPC prerequisites for Windows](https://github.com/grpc/grpc/blob/master/BUILDING.md#windows)
installed as well.

## Build without CMake
When building without CMake, you must manually build and install 
gRPC as [described here](https://grpc.io/docs/quickstart/cpp/).
Once that is done, generate the gRPC/protobuf files by running
protoc:

```bash
DTHKE_GRPC_DIR=<path/to/sdk-diatheke/grpc>
GRPC_INSTALL_DIR=<path/to/grpc/install>
OUTDIR=<path/to/generated/output>
mkdir -p $OUTDIR

PROTO_INCLUDE_PATH=$GRPC_INSTALL_DIR/include/google/protobuf

# Generate files for Diatheke
$GRPC_INSTALL_DIR/bin/protoc \
  -I $PROTO_INCLUDE_PATH:$DTHKE_GRPC_DIR \
  --grpc_out=$OUTDIR \
  --cpp_out=$OUTDIR \
  --plugin=protoc-gen-grpc=$GRPC_INSTALL_DIR/bin/grpc_cpp_plugin \
  $DTHKE_GRPC_DIR/diatheke.proto
```

Once the files are generated, include them with the wrapper code
from this directory in your project. Then be sure to link your
binaries with libgrpc, libgrpc++, and libprotobuf (found in
the gRPC installation's `lib` directory).
