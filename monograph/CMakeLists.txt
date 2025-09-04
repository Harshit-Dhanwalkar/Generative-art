cmake_minimum_required(VERSION 3.10)
project(Monograph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

add_executable(monograph_app
main.cpp
Monograph.hpp
../lib/Palettes.hpp
)

target_link_libraries(monograph_app PUBLIC
sfml-graphics
sfml-window
sfml-system
)
