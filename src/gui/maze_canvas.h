#pragma once
// MazeCanvas — the drawing surface. paintEvent paints the current matrix plus
// whichever replay overlays are active (generation cursor, BFS frontier,
// checked edge, route). It owns NO core data: every pointer here is a
// non-owning view into a MainWindow member.

#include <QWidget>
#include "maze_matrix.h"
#include "linked_list.h"

class QPaintEvent;

class MazeCanvas : public QWidget {
    Q_OBJECT
public:
    explicit MazeCanvas(QWidget* parent = 0);

    void setMaze(const MazeMatrix* maze);
    void setGenerationCursor(int row, int col);                     // -1 hides it
    void setVisited(const LinkedList<int>* vertices, int count);    // dequeued so far
    void setCurrentEdge(int fromVertex, int toVertex);              // -1,-1 hides it
    void setRoute(const LinkedList<int>* route, int count);         // route tail so far
    void setPlayer(int vertex);                                     // -1 hides the pawn
    void setTrail(const LinkedList<int>* trail);                    // player breadcrumbs
    void setBlockedWall(int row, int col, int side);                // -1 clears the flash
    void resetOverlays();

signals:
    void moveRequested(int side);   // emitted for each arrow/WASD press in play mode

protected:
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    const MazeMatrix* maze_;
    int cursorRow_;
    int cursorCol_;
    const LinkedList<int>* visited_;
    int visitedCount_;
    int edgeFrom_;
    int edgeTo_;
    const LinkedList<int>* route_;
    int routeCount_;

    // manual play mode (walking the maze with the keyboard)
    int playerVertex_;              // current cell id, -1 = not playing
    const LinkedList<int>* trail_;  // breadcrumbs list owned by MainWindow
    int blockedRow_;                // wall the last move bounced off (red flash)
    int blockedCol_;
    int blockedSide_;
};
