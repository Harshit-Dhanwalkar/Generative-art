cmake_minimum_required(VERSION 3.10)
project(ParticleSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system) # audio

file(COPY ${CMAKE_SOURCE_DIR}/fonts/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts/)

add_executable(particlesystem_app
    main.cpp
    ../lib/Palettes.hpp
    particle.cpp
    particle.hpp
)

target_link_libraries(particlesystem_app PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    # sfml-audio
)
