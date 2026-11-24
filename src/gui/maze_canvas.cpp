#include "maze_canvas.h"
#include <QKeyEvent>
#include <QPainter>

namespace {


const char* kBombPix[8] = {
    "......S.",
    ".....S..",
    "....F...",
    "...F....",
    "..BBBB..",
    ".BBBBBB.",
    ".BBBBBB.",
    "..BBBB..",
};
const char* kKeyPix[8] = {
    ".KKK....",
    ".K.K....",
    ".KKK....",
    "..K.....",
    "..KK....",
    "..K.....",
    "..KK....",
    "..K.....",
};

QColor spriteColorFor(char c)
{
    switch (c) {
        case 'B': return QColor(25, 25, 25);
        case 'F': return QColor(120, 90, 50);
        case 'S': return QColor(255, 150, 0);
        case 'K': return QColor(240, 190, 40);
    }
    return QColor();
}

void drawPixelSprite(QPainter& painter, const char* pixels[8], int x0, int y0, int cell)
{
    int px = cell / 8;
    if (px < 2)
        px = 2;
    int ox = x0 + (cell - 8 * px) / 2;
    int oy = y0 + (cell - 8 * px) / 2;
    if (ox < x0) ox = x0;
    if (oy < y0) oy = y0;
    painter.setPen(Qt::NoPen);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            char c = pixels[y][x];
            if (c == '.')
                continue;
            painter.fillRect(QRect(ox + x * px, oy + y * px, px, px), spriteColorFor(c));
        }
    }
}
}

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
    , playerVertex_(-1)
    , trail_(0)
    , blockedRow_(-1)
    , blockedCol_(-1)
    , blockedSide_(-1)
    , mines_(0)
    , keys_(0)
    , explosionVertex_(-1)
{
    setMinimumSize(360, 360);
    setMouseTracking(false);
    setFocusPolicy(Qt::StrongFocus);
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

void MazeCanvas::setVisited(const IntLinkedList* vertices, int count)
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

void MazeCanvas::setRoute(const IntLinkedList* route, int count)
{
    route_ = route;
    routeCount_ = count;
    update();
}

void MazeCanvas::setPlayer(int vertex)
{
    playerVertex_ = vertex;
    update();
}

void MazeCanvas::setTrail(const IntLinkedList* trail)
{
    trail_ = trail;
    update();
}

void MazeCanvas::setBlockedWall(int row, int col, int side)
{
    blockedRow_ = row;
    blockedCol_ = col;
    blockedSide_ = side;
    update();
}

void MazeCanvas::setMines(const IntLinkedList* mines)
{
    mines_ = mines;
    update();
}

void MazeCanvas::setKeys(const IntLinkedList* keys)
{
    keys_ = keys;
    update();
}

void MazeCanvas::setExplosion(int vertex)
{
    explosionVertex_ = vertex;
    update();
}

void MazeCanvas::keyPressEvent(QKeyEvent* event)
{
    int side = 0;
    switch (event->key()) {
        case Qt::Key_Up:    case Qt::Key_W: side = MazeMatrix::WALL_N; break;
        case Qt::Key_Right: case Qt::Key_D: side = MazeMatrix::WALL_E; break;
        case Qt::Key_Down:  case Qt::Key_S: side = MazeMatrix::WALL_S; break;
        case Qt::Key_Left:  case Qt::Key_A: side = MazeMatrix::WALL_W; break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
    if (playerVertex_ >= 0)
        emit moveRequested(side);
    event->accept();
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

void MazeCanvas::paintEvent(QPaintEvent* )
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


    painter.fillRect(QRect(offX, offY, cell * w, cell * h), QColor(255, 255, 255));


    if (visited_ != 0) {
        painter.setPen(Qt::NoPen);
        IntLinkedList::Cursor it = visited_->cursor();
        for (int i = 0; i < visitedCount_ && it.hasNext(); ++i) {
            int v = it.next();
            int r = v / w;
            int c = v % w;
            painter.fillRect(QRect(offX + c * cell + 2, offY + r * cell + 2,
                                   cell - 4, cell - 4), QColor(110, 200, 110));
        }
    }


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


    if (route_ != 0) {
        painter.setPen(Qt::NoPen);
        IntLinkedList::Cursor it = route_->cursor();
        for (int i = 0; i < routeCount_ && it.hasNext(); ++i) {
            int v = it.next();
            int r = v / w;
            int c = v % w;
            painter.fillRect(QRect(offX + c * cell + cell / 4, offY + r * cell + cell / 4,
                                   cell / 2, cell / 2), QColor(215, 70, 70));
        }
    }


    if (trail_ != 0) {
        painter.setPen(Qt::NoPen);
        IntLinkedList::Cursor it = trail_->cursor();
        while (it.hasNext()) {
            int v = it.next();
            int r = v / w;
            int c = v % w;
            painter.fillRect(QRect(offX + c * cell + cell / 3, offY + r * cell + cell / 3,
                                   cell / 3, cell / 3), QColor(120, 180, 250, 120));
        }
    }


    if (mines_ != 0) {
        IntLinkedList::Cursor it = mines_->cursor();
        while (it.hasNext()) {
            int v = it.next();
            drawPixelSprite(painter, kBombPix, offX + (v % w) * cell, offY + (v / w) * cell, cell);
        }
    }
    if (keys_ != 0) {
        IntLinkedList::Cursor it = keys_->cursor();
        while (it.hasNext()) {
            int v = it.next();
            drawPixelSprite(painter, kKeyPix, offX + (v % w) * cell, offY + (v / w) * cell, cell);
        }
    }


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


    if (blockedSide_ > 0 && blockedRow_ >= 0) {
        int x0 = offX + blockedCol_ * cell;
        int y0 = offY + blockedRow_ * cell;
        painter.setPen(QPen(QColor(225, 40, 40), 4));
        switch (blockedSide_) {
            case MazeMatrix::WALL_N:
                painter.drawLine(QPoint(x0, y0), QPoint(x0 + cell, y0)); break;
            case MazeMatrix::WALL_E:
                painter.drawLine(QPoint(x0 + cell, y0), QPoint(x0 + cell, y0 + cell)); break;
            case MazeMatrix::WALL_S:
                painter.drawLine(QPoint(x0, y0 + cell), QPoint(x0 + cell, y0 + cell)); break;
            case MazeMatrix::WALL_W:
                painter.drawLine(QPoint(x0, y0), QPoint(x0, y0 + cell)); break;
        }
    }


    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(offX + cell / 3, offY - 3, cell / 3, 7), QColor(70, 180, 70));
    painter.fillRect(QRect(offX + (w - 1) * cell + cell / 3,
                           offY + h * cell - 4, cell / 3, 7), QColor(70, 70, 230));


    if (cursorRow_ >= 0) {
        painter.setPen(QPen(QColor(255, 160, 0), 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPoint(offX + cursorCol_ * cell + cell / 2,
                                   offY + cursorRow_ * cell + cell / 2),
                            cell / 3, cell / 3);
    }


    if (playerVertex_ >= 0) {
        int r = playerVertex_ / w;
        int c = playerVertex_ % w;
        painter.setPen(QPen(QColor(150, 60, 0), 2));
        painter.setBrush(QColor(255, 130, 10));
        painter.drawEllipse(QPoint(offX + c * cell + cell / 2,
                                   offY + r * cell + cell / 2),
                            cell / 3, cell / 3);
    }


    if (explosionVertex_ >= 0) {
        int r = explosionVertex_ / w;
        int c = explosionVertex_ % w;
        int cx = offX + c * cell + cell / 2;
        int cy = offY + r * cell + cell / 2;
        painter.setPen(QPen(QColor(255, 90, 0), 3));
        painter.setBrush(QColor(255, 170, 0, 190));
        painter.drawEllipse(QPoint(cx, cy), cell / 2, cell / 2);
        painter.setBrush(Qt::NoBrush);
        const int rays[8][2] = { {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1} };
        for (int i = 0; i < 8; ++i) {
            painter.drawLine(QPoint(cx + rays[i][0] * cell / 2, cy + rays[i][1] * cell / 2),
                             QPoint(cx + rays[i][0] * cell * 3 / 5, cy + rays[i][1] * cell * 3 / 5));
        }
    }
}
