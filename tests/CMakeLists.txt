add_executable(maze_tests
    test_main.cpp
    test_matrix.cpp
    test_lists.cpp
    test_generator.cpp
    test_graph_bfs.cpp
    test_extra.cpp
    test_io.cpp
    test_library.cpp)
target_link_libraries(maze_tests PRIVATE maze_core)
# Fully self-contained exe: no MinGW runtime DLLs (libstdc++/libgcc/winpthread)
# are needed at run time, so double-clicking just works.
target_link_options(maze_tests PRIVATE -static)
