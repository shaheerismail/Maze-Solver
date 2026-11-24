#pragma once





#include <QWidget>
#include "maze_matrix.h"
#include "int_linked_list.h"

class QPaintEvent;

class MazeCanvas : public QWidget {
    Q_OBJECT
public:
    explicit MazeCanvas(QWidget* parent = 0);

    void setMaze(const MazeMatrix* maze);
    void setGenerationCursor(int row, int col);
    void setVisited(const IntLinkedList* vertices, int count);
    void setCurrentEdge(int fromVertex, int toVertex);
    void setRoute(const IntLinkedList* route, int count);
    void setPlayer(int vertex);
    void setTrail(const IntLinkedList* trail);
    void setBlockedWall(int row, int col, int side);
    void setMines(const IntLinkedList* mines);
    void setKeys(const IntLinkedList* keys);
    void setExplosion(int vertex);
    void resetOverlays();

signals:
    void moveRequested(int side);

protected:
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    const MazeMatrix* maze_;
    int cursorRow_;
    int cursorCol_;
    const IntLinkedList* visited_;
    int visitedCount_;
    int edgeFrom_;
    int edgeTo_;
    const IntLinkedList* route_;
    int routeCount_;


    int playerVertex_;
    const IntLinkedList* trail_;
    int blockedRow_;
    int blockedCol_;
    int blockedSide_;
    const IntLinkedList* mines_;
    const IntLinkedList* keys_;
    int explosionVertex_;
};
