add_library(maze_core STATIC
    maze_matrix.cpp
    rng.cpp
    maze_generator.cpp
    maze_graph.cpp
    bfs_solver.cpp
    maze_library.cpp
    maze_io.cpp)
target_include_directories(maze_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(maze_core PUBLIC cxx_std_17)
