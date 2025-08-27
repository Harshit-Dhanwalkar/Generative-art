cmake_minimum_required(VERSION 3.10)
project(GenerativeArt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

add_executable(generative_art
    main.cpp
    Palettes.hpp
    GIFRecorder.hpp
    AudioVisualizer.hpp
)

target_link_libraries(generative_art PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)
