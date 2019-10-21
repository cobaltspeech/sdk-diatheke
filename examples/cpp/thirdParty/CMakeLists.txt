# Copyright (2019) Cobalt Speech and Language, Inc.

cmake_minimum_required(VERSION 3.5.0)
project(diatheke-examples-third-party)

# Download cpptoml (for config file parsing).
if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/cpptoml)
    find_package(Git REQUIRED)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --recurse-submodules -b v0.1.1 https://github.com/skystrife/cpptoml
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE CPPTOML_CLONE_RESULT
        OUTPUT_VARIABLE CPPTOML_CLONE_OUTPUT
        ERROR_VARIABLE CPPTOML_CLONE_ERROR)

    if(NOT ${CPPTOML_CLONE_RESULT} EQUAL 0)
        message(FATAL_ERROR "could not clone cpptoml\n"
            "${CPPTOML_CLONE_OUTPUT}\n" "${CPPTOML_CLONE_ERROR}\n\n"
            "Please make sure Git is configured correctly or manually install cpptoml")
    endif()
endif()
set(CPPTOML_BUILD_EXAMPLES OFF CACHE BOOL "") # Disable cpptoml examples
add_subdirectory(cpptoml)
