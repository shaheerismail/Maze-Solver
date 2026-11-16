add_library(maze_core STATIC
    int_linked_list.cpp
    int_linked_queue.cpp
    carve_step_list.cpp
    solve_step_list.cpp
    maze_record_list.cpp
    string_list.cpp
    maze_matrix.cpp
    rng.cpp
    maze_generator.cpp
    maze_graph.cpp
    bfs_solver.cpp
    maze_library.cpp
    maze_io.cpp)
target_include_directories(maze_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(maze_core PUBLIC cxx_std_17)
