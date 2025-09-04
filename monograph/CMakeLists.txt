cmake_minimum_required(VERSION 3.10)
project(Monograph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

file(COPY ${CMAKE_SOURCE_DIR}/fonts/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts/)

add_executable(monograph
    main.cpp
    Monograph.hpp
    ../lib/Palettes.hpp
)

target_link_libraries(monograph PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
)
