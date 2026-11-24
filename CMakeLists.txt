cmake_minimum_required(VERSION 3.19)
project(MazeSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/core)

add_subdirectory(tests)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
add_subdirectory(src/gui)
