cmake_minimum_required(VERSION 3.10)
project(GenerativeArt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

option(BUILD_MONOGRAPH "Build the Monograph project" ON)
option(BUILD_AUDIO_VISUALIZER "Build the Audio Visualizer project" ON)

if(BUILD_AUDIO_VISUALIZER)
add_subdirectory(audiovisualizer)
endif()

if(BUILD_MONOGRAPH)
add_subdirectory(monograph)
endif()
