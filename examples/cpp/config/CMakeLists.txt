# Copyright (2019) Cobalt Speech and Language, Inc.

cmake_minimum_required(VERSION 3.5.0)
project(diatheke-examples-config)

# Static library so we don't have to worry about shared libraries
# being on the path for our demo executables. It can all be compiled
# into the final executable.
add_library(demo_config STATIC
    demo_config.cpp
    demo_config.h)

target_include_directories(demo_config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(demo_config PUBLIC 
    cpptoml)
