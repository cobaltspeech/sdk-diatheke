# Copyright (2019) Cobalt Speech and Language, Inc.

cmake_minimum_required(VERSION 3.5.0)
project(diatheke-audio-io)

# Static library so we don't have to worry about shared libraries
# being on the path for our demo executables. It can all be compiled
# into the final executable.
add_library(audio_io STATIC
    player.cpp
    player.h
    recorder.cpp
    recorder.h)

target_include_directories(audio_io PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})
