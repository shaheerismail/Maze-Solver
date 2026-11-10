add_executable(maze_tests
    test_main.cpp
    test_matrix.cpp)
target_link_libraries(maze_tests PRIVATE maze_core)
