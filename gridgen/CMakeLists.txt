cmake_minimum_required(VERSION 3.10)
project(Gridgen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

file(COPY ${CMAKE_SOURCE_DIR}/fonts/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts/)

add_executable(gridgen_app
    main.cpp
    GridGen.hpp
    ../lib/Palettes.hpp
)

target_link_libraries(gridgen_app PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(gridgen_app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
