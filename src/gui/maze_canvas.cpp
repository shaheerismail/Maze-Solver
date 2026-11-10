#include "maze_canvas.h"
#include <QPainter>

MazeCanvas::MazeCanvas(QWidget* parent)
    : QWidget(parent)
    , maze_(0)
    , cursorRow_(-1)
    , cursorCol_(-1)
    , visited_(0)
    , visitedCount_(0)
    , edgeFrom_(-1)
    , edgeTo_(-1)
    , route_(0)
    , routeCount_(0)
{
    setMinimumSize(360, 360);
    setMouseTracking(false);
}

void MazeCanvas::setMaze(const MazeMatrix* maze)
{
    maze_ = maze;
    update();
}

void MazeCanvas::setGenerationCursor(int row, int col)
{
    cursorRow_ = row;
    cursorCol_ = col;
    update();
}

void MazeCanvas::setVisited(const LinkedList<int>* vertices, int count)
{
    visited_ = vertices;
    visitedCount_ = count;
    update();
}

void MazeCanvas::setCurrentEdge(int fromVertex, int toVertex)
{
    edgeFrom_ = fromVertex;
    edgeTo_ = toVertex;
    update();
}

void MazeCanvas::setRoute(const LinkedList<int>* route, int count)
{
    route_ = route;
    routeCount_ = count;
    update();
}

void MazeCanvas::resetOverlays()
{
    cursorRow_ = -1;
    cursorCol_ = -1;
    visited_ = 0;
    visitedCount_ = 0;
    edgeFrom_ = -1;
    edgeTo_ = -1;
    route_ = 0;
    routeCount_ = 0;
    update();
}

void MazeCanvas::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(245, 245, 245));
    if (maze_ == 0 || maze_->rows() == 0)
        return;

    const int h = maze_->rows();
    const int w = maze_->cols();
    int cw = (width() - 16) / (w > 0 ? w : 1);
    int ch = (height() - 16) / (h > 0 ? h : 1);
    int cell = cw < ch ? cw : ch;
    if (cell < 5)
        cell = 5;
    int offX = (width() - cell * w) / 2;
    int offY = (height() - cell * h) / 2;

    // 1) room fills first, so walls are painted on top
    painter.fillRect(QRect(offX, offY, cell * w, cell * h), QColor(255, 255, 255));

    // 2) BFS visited cells (green) — the frontier Microsoft-style trace
    if (visited_ != 0) {
        painter.setPen(Qt::NoPen);
        LinkedList<int>::Iterator it = visited_->iterator();
        for (int i = 0; i < visitedCount_ && it.hasNext(); ++i) {
            int v = it.next();
            int r = v / w;
            int c = v % w;
            painter.fillRect(QRect(offX + c * cell + 2, offY + r * cell + 2,
                                   cell - 4, cell - 4), QColor(110, 200, 110));
        }
    }

    // 3) current edge being examined (yellow) — the classical BFS probe
    if (edgeTo_ >= 0) {
        int r = edgeTo_ / w;
        int c = edgeTo_ % w;
        painter.setPen(Qt::NoPen);
        painter.fillRect(QRect(offX + c * cell + 2, offY + r * cell + 2,
                               cell - 4, cell - 4), QColor(250, 210, 80));
        painter.setPen(QPen(QColor(180, 140, 30), 2));
        int fr = edgeFrom_ / w;
        int fc = edgeFrom_ % w;
        painter.drawLine(QPoint(offX + fc * cell + cell / 2, offY + fr * cell + cell / 2),
                         QPoint(offX + c * cell + cell / 2, offY + r * cell + cell / 2));
    }

    // 4) route so far (red) — entrance -> exit
    if (route_ != 0) {
        painter.setPen(Qt::NoPen);
        LinkedList<int>::Iterator it = route_->iterator();
        for (int i = 0; i < routeCount_ && it.hasNext(); ++i) {
            int v = it.next();
            int r = v / w;
            int c = v % w;
            painter.fillRect(QRect(offX + c * cell + cell / 4, offY + r * cell + cell / 4,
                                   cell / 2, cell / 2), QColor(215, 70, 70));
        }
    }

    // 5) walls
    painter.setPen(QPen(QColor(55, 55, 55), 2));
    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            int x0 = offX + c * cell;
            int y0 = offY + r * cell;
            if (maze_->hasWall(r, c, MazeMatrix::WALL_N))
                painter.drawLine(QPoint(x0, y0), QPoint(x0 + cell, y0));
            if (maze_->hasWall(r, c, MazeMatrix::WALL_E))
                painter.drawLine(QPoint(x0 + cell, y0), QPoint(x0 + cell, y0 + cell));
            if (maze_->hasWall(r, c, MazeMatrix::WALL_S))
                painter.drawLine(QPoint(x0, y0 + cell), QPoint(x0 + cell, y0 + cell));
            if (maze_->hasWall(r, c, MazeMatrix::WALL_W))
                painter.drawLine(QPoint(x0, y0), QPoint(x0, y0 + cell));
        }
    }

    // 6) entrance / exit markers on the outside openings
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(offX + cell / 3, offY - 3, cell / 3, 7), QColor(70, 180, 70));
    painter.fillRect(QRect(offX + (w - 1) * cell + cell / 3,
                           offY + h * cell - 4, cell / 3, 7), QColor(70, 70, 230));

    // 7) generation cursor: the cell the backtracker is carving
    if (cursorRow_ >= 0) {
        painter.setPen(QPen(QColor(255, 160, 0), 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPoint(offX + cursorCol_ * cell + cell / 2,
                                   offY + cursorRow_ * cell + cell / 2),
                            cell / 3, cell / 3);
    }
}
