cmake_minimum_required(VERSION 3.10)
project(TruchetTiles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

file(COPY ${CMAKE_SOURCE_DIR}/fonts/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts/)

add_executable(smithtiles_app
    main.cpp
    SmithTile.cpp
    ../lib/Palettes.hpp
)

target_link_libraries(smithtiles_app PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(smithtiles_app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
