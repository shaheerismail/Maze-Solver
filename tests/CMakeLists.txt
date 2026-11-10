add_executable(maze_tests
    test_main.cpp
    test_matrix.cpp
    test_lists.cpp
    test_generator.cpp
    test_graph_bfs.cpp)
target_link_libraries(maze_tests PRIVATE maze_core)
# Self-contained exe: runs by double-click without MinGW DLLs on PATH.
target_link_options(maze_tests PRIVATE -static-libgcc -static-libstdc++)
