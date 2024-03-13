cmake_minimum_required(VERSION 3.26)

project(
    HamArc
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(bin)
add_subdirectory(parser)
add_subdirectory(lib)