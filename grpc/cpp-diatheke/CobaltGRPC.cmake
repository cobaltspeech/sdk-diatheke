# Copyright (2019) Cobalt Speech and Language, Inc.
#
# This CMake file automatically adds gRPC to the project if it hasn't
# already been added somewhere. It also defines some useful functions
# to help us run protoc and generate c++ file from the .proto files.


# This allows cross-compiling to work - a native version of protoc is required.
set(GRPC_NATIVE_DIR "" CACHE PATH "Path to native gRPC installation for cross compiling.")

# Check if we have already added gRPC to the project. If not, we will
# add it here.
if(NOT TARGET grpc)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/grpc)
        # Download grpc. We could do this as a git submodule instead, but
        # I haven't had a lot of good experiences with them. Also this will
        # still work even if the SDK isn't part of a git repository (code
        # was downloaded as a zip/tar file, or maybe a user just copied it
        # into their project).

        # Get the specific version of gRPC we used for our wrapper code.
        find_package(Git REQUIRED)

        message(STATUS "Downloading gRPC...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone --recurse-submodules -b v1.22.0 https://github.com/grpc/grpc
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE _git_clone_result
            OUTPUT_VARIABLE _git_clone_output
            ERROR_VARIABLE _git_clone_error)

        # If there was an error cloning, print it out to the user
        if(NOT ${_git_clone_result} EQUAL 0)
            message(FATAL_ERROR "could not clone grpc\n"
                "${_git_clone_output}\n\n" "${_git_clone_error}\n\n")
        endif()
    endif()

    # Disable these tests for one of gRPC's third party libraries
    # (won't build with c++11).
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")

    # Turn off the warning-as-error flag for some warnings. For some
    # compilers, grpc won't build otherwise.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=shadow -Wno-error=attributes")

    # If we are cross-compiling, add the native grpc dir to the CMAKE_PREFIX_PATH
    # so that the grpc build can find it using find_program
    if(EXISTS ${GRPC_NATIVE_DIR})
        set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GRPC_NATIVE_DIR})
    endif()

    # Add grpc as a subproject here.
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/grpc)
endif()

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
