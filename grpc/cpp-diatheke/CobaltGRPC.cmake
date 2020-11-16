# Copyright (2020) Cobalt Speech and Language, Inc.
#
# This CMake file automatically adds gRPC to the project if it hasn't
# already been added somewhere. It also defines some useful functions
# to help us run protoc and generate c++ file from the .proto files.


# This allows cross-compiling to work - a native version of protoc is required
# (i.e., a version of protoc that will run on the system doing the compiling).
set(GRPC_NATIVE_DIR "" CACHE PATH "Path to native gRPC installation for cross compiling.")

# If we are cross-compiling, add the native grpc dir to the CMAKE_PREFIX_PATH
# so that the grpc build can find it using find_program
if(EXISTS ${GRPC_NATIVE_DIR})
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GRPC_NATIVE_DIR})
endif()

# Download gRPC from github and add it as part of the project
include(FetchContent)
FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.33.2
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(gRPC)

# Check if we have already defined the run_protoc function
if(NOT COMMAND run_protoc)

    # Define a function to conveniently run protoc. Note that the order
    # of PROTO files is important when running the function.
    function(run_protoc)
        set(options "")
        set(oneValueArgs OUTPUT_DIR OUTPUT_SUBDIR RESULT_FILES)
        set(multiValueArgs PROTOS)
        cmake_parse_arguments(RUN_PROTOC "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN})

        # Make sure the output directory exists
        file(MAKE_DIRECTORY ${RUN_PROTOC_OUTPUT_DIR})

        # Formulate the actual directory where the generated files exist
        # (depending on the proto file, sometimes protoc will output to
        # a subdirectory within the specified output directory).
        set(OUTDIR "${RUN_PROTOC_OUTPUT_DIR}")
        if(RUN_PROTOC_OUTPUT_SUBDIR)
            set(OUTDIR "${OUTDIR}/${RUN_PROTOC_OUTPUT_SUBDIR}")
        endif()

        # Setup the generated filenames for each proto file
        # Get the include path for each proto file
        set(PROTOC_INCLUDE_LIST
            "${grpc_SOURCE_DIR}/third_party/protobuf/src"
            "${grpc_SOURCE_DIR}/third_party/googleapis")

        foreach(protofile ${RUN_PROTOC_PROTOS})
            get_filename_component(absolute_proto "${protofile}" ABSOLUTE)
            get_filename_component(base_name "${absolute_proto}" NAME_WE)
            get_filename_component(proto_path "${absolute_proto}" PATH)

            list(APPEND PROTOC_INCLUDE_LIST "${proto_path}")
            list(APPEND ALL_PROTOS "${absolute_proto}")

            list(APPEND ${RUN_PROTOC_RESULT_FILES}
                 "${OUTDIR}/${base_name}.grpc.pb.cc"
                 "${OUTDIR}/${base_name}.grpc.pb.h"
                 "${OUTDIR}/${base_name}.pb.cc"
                 "${OUTDIR}/${base_name}.pb.h")
        endforeach()

        # Uniquify the include path list and format it properly
        list(REMOVE_DUPLICATES PROTOC_INCLUDE_LIST)
        string(REPLACE ";" ":" PROTOC_INCLUDES "${PROTOC_INCLUDE_LIST}")

        # To enable cross-compiling, we allow users to specify a native
        # grpc installation, which we can use to run protoc. The version
        # of this native grpc should be the same as the one used for
        # cross compiling.
        set(_PROTOC_CMD $<TARGET_FILE:protoc>)
        set(_GRPC_CPP_PLUGIN $<TARGET_FILE:grpc_cpp_plugin>)
        if(EXISTS ${GRPC_NATIVE_DIR})
            get_filename_component(absolute_grpc_native "${GRPC_NATIVE_DIR}" ABSOLUTE)
            set(_PROTOC_CMD "${absolute_grpc_native}/bin/protoc")
            set(_GRPC_CPP_PLUGIN "${absolute_grpc_native}/bin/grpc_cpp_plugin")
        endif()

        add_custom_command(
            OUTPUT ${${RUN_PROTOC_RESULT_FILES}}
            COMMAND ${_PROTOC_CMD}
            ARGS -I ${PROTOC_INCLUDES}
              --grpc_out=${RUN_PROTOC_OUTPUT_DIR}
              --cpp_out=${RUN_PROTOC_OUTPUT_DIR}
              --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN}
              ${ALL_PROTOS}
            DEPENDS ${ALL_PROTOS})

        # Make sure the output variable specified makes it back to the
        # parent scope.
        set(${RUN_PROTOC_RESULT_FILES} ${${RUN_PROTOC_RESULT_FILES}} PARENT_SCOPE)

    endfunction(run_protoc)
endif()
