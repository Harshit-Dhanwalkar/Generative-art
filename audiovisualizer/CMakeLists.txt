cmake_minimum_required(VERSION 3.10)
project(AudioVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

add_executable(audiovisualizer_app
main.cpp
../lib/Palettes.hpp
GIFRecorder.hpp
AudioVisualizer.hpp
)

target_link_libraries(audiovisualizer_app PUBLIC
sfml-graphics
sfml-window
sfml-system
sfml-audio
)
