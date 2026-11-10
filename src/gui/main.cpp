#include <QApplication>
#include <QWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Maze Solver");
    window.resize(960, 720);
    window.show();
    return app.exec();
}
