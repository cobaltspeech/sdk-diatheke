# Copyright (2019) Cobalt Speech and Language, Inc.

cmake_minimum_required(VERSION 3.5.0)
project(diatheke-cpp-examples)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# Add the diatheke client code as a subdirectory
add_subdirectory(../../grpc/cpp-diatheke cpp-diatheke)

# Add third party dependencies
add_subdirectory(thirdParty)

# Add the audio handling code
add_subdirectory(audioIO)

# Add configuration code
add_subdirectory(config)

set(COMMON_LIBS
    diatheke_client
    demo_config
    audio_io)

# Create the demo executables
# Audio interaction only
add_executable(demo_audio audio_main.cpp)
target_link_libraries(demo_audio PRIVATE ${COMMON_LIBS})


# Command-line interaction only
add_executable(demo_cli cli_main.cpp)
target_link_libraries(demo_cli PRIVATE diatheke_client demo_config)


# Demo using ASR via Diatheke
add_executable(demo_asr asr_main.cpp)
target_link_libraries(demo_asr PRIVATE ${COMMON_LIBS})


# Demo using TTS via Diatheke
add_executable(demo_tts tts_main.cpp)
target_link_libraries(demo_tts PRIVATE ${COMMON_LIBS})
