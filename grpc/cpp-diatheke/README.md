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

to your project's CMakeLists.txt.

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
