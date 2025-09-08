cmake_minimum_required(VERSION 3.10)
project(GabrielsHorn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

add_executable(gabriels_horn
    main.cpp
)

target_link_libraries(gabriels_horn PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
)

file(COPY ${CMAKE_SOURCE_DIR}/fonts/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts/)
