add_executable(maze_gui WIN32
    main.cpp)
target_link_libraries(maze_gui PRIVATE maze_core Qt6::Widgets)
