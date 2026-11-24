#pragma once











#include <QWidget>
#include "maze_matrix.h"
#include "maze_generator.h"
#include "maze_graph.h"
#include "bfs_solver.h"
#include "maze_library.h"
#include "int_linked_list.h"
#include "carve_step_list.h"
#include "solve_step_list.h"

class QSpinBox;
class QPushButton;
class QTimer;
class QLabel;
class MazeCanvas;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void onGenerate();
    void onSolve();
    void onPlay();
    void onMoveRequested(int side);
    void onSave();
    void onLoad();
    void onInsertInLibrary();
    void onSearchLibrary();
    void onFrame();

private:

    MazeMatrix maze_;
    MazeMatrix animMatrix_;
    MazeGraph graph_;
    BfsSolver solver_;
    MazeLibrary library_;


    MazeCanvas* canvas_;
    QSpinBox* rowsInput_;
    QSpinBox* colsInput_;
    QSpinBox* seedInput_;
    QSpinBox* extraOpeningsInput_;
    QSpinBox* librarySeedInput_;
    QPushButton* generateButton_;
    QPushButton* solveButton_;
    QPushButton* playButton_;
    QPushButton* saveButton_;
    QPushButton* loadButton_;
    QPushButton* insertButton_;
    QPushButton* searchButton_;
    QLabel* status_;
    QTimer* timer_;


    CarveStepList genSteps_;
    SolveStepList solveSteps_;
    CarveStepList::Cursor genCursor_;
    SolveStepList::Cursor solveCursor_;
    IntLinkedList visited_;
    IntLinkedList route_;
    int playhead_;
    int phase_;
    int stepsPerTick_;
    int visitedCount_;
    int routeCount_;
    bool routeFound_;


    bool playMode_;
    bool won_;
    int playerVertex_;
    int moves_;
    IntLinkedList playerTrail_;



    IntLinkedList mines_;
    IntLinkedList keysPlaced_;
    IntLinkedList keysLeft_;
    int keysTotal_;
    int keysCollected_;
    bool dead_;


    unsigned int activeSeed_;
    int activeExtra_;


    void stopAnimation();
    void announce(const QString& text);
    MazeRecord recordOfCurrentMaze();
    void placeKeysAndMines(unsigned int seed);
    bool reachableAvoidingMines(int target);
    static int starsFor(int collected, int total);
};
