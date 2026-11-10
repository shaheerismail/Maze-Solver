cmake_minimum_required(VERSION 3.19)
project(MazeSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt-free data structure / algorithm library, built and testable headless.
add_subdirectory(src/core)

# Console test executable (also a --demo ASCII gallery later on).
add_subdirectory(tests)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
add_subdirectory(src/gui)
