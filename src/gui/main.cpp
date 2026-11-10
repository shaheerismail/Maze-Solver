#include <QApplication>
#include "main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Maze Solver");
    app.setOrganizationName("DSA Course Project");
    MainWindow window;
    window.setWindowTitle("Maze Solver — maze generator + BFS shortest route");
    window.resize(1150, 720);
    window.show();
    return app.exec();
}
