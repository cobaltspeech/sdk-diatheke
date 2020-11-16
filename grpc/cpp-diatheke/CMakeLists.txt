# Copyright (2020) Cobalt Speech and Language, Inc.

cmake_minimum_required(VERSION 3.14.0)
project(diatheke_client)

include(CobaltGRPC.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate the grpc files from our proto files.
run_protoc(RESULT_FILES DIATHEKE_PROTOFILES
    PROTOS ../diatheke.proto
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Setup the library
add_library(diatheke_client
    ${DIATHEKE_PROTOFILES}
    diatheke_asr_stream.cpp
    diatheke_asr_stream.h
    diatheke_client_error.h
    diatheke_client_error.cpp
    diatheke_client.cpp
    diatheke_client.h
    diatheke_tts_stream.cpp
    diatheke_tts_stream.h
)

# Setup the linking and include directories for the library.
# We give it PUBLIC visibility so other targets can use the
# include directories and libraries.
target_include_directories(diatheke_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(diatheke_client PUBLIC
    grpc grpc++)
