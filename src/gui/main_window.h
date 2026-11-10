#pragma once
// MainWindow — the Qt layer of the project. It owns the core objects as
// members (no manual new/delete anywhere), drives them once per button click,
// and replays the recorded step lists with a 20 ms QTimer.
//
// Replay model:
//   phase 1: generation — GenSteps are applied to animMatrix_ (a blank copy),
//            so the maze visibly gets carved cell by cell;
//   phase 2: BFS search — SolveSteps are replayed; dequeued vertices tint
//            green, the edge being checked flashes yellow;
//   phase 3: route — the final BFS route is drawn from entrance to exit.

#include <QWidget>
#include "maze_matrix.h"
#include "maze_generator.h"
#include "maze_graph.h"
#include "bfs_solver.h"
#include "maze_library.h"
#include "linked_list.h"

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
    void onPlay();               // manual walk mode (arrow keys / WASD)
    void onMoveRequested(int side);
    void onSave();
    void onLoad();
    void onInsertInLibrary();
    void onSearchLibrary();
    void onFrame();     // the single QTimer tick

private:
    // ---- core objects (values; the GUI never allocates state manually) ----
    MazeMatrix maze_;        // authoritative maze
    MazeMatrix animMatrix_;  // blank copy that generation replay carves
    MazeGraph graph_;
    BfsSolver solver_;
    MazeLibrary library_;

    // ---- widgets ----
    MazeCanvas* canvas_;
    QSpinBox* rows_;
    QSpinBox* cols_;
    QSpinBox* seed_;
    QSpinBox* extra_;
    QSpinBox* librarySeed_;
    QPushButton* generateButton_;
    QPushButton* solveButton_;
    QPushButton* playButton_;
    QPushButton* saveButton_;
    QPushButton* loadButton_;
    QPushButton* insertButton_;
    QPushButton* searchButton_;
    QLabel* status_;
    QTimer* timer_;

    // ---- replay state ----
    LinkedList<GenStep> genSteps_;
    LinkedList<SolveStep> solveSteps_;
    LinkedList<GenStep>::Iterator genCursor_;      // walk genSteps_ during phase 1
    LinkedList<SolveStep>::Iterator solveCursor_;  // walk solveSteps_ during phase 2
    LinkedList<int> visited_;                      // dequeued order, grows during replay
    LinkedList<int> route_;
    int playhead_;
    int phase_;                                    // 0 idle / 1 gen / 2 search / 3 route
    int stepsPerTick_;                             // ~4 s animations at any maze size
    int visitedCount_;
    int routeCount_;
    bool routeFound_;

    // ---- manual play mode (walk the maze with the keyboard) ----
    bool playMode_;             // true between pressing Play and winning/leaving
    bool won_;                  // exit reached; keys ignored afterwards
    int playerVertex_;          // pawn's current cell id (-1 = not playing)
    int moves_;                 // successful moves so far
    LinkedList<int> playerTrail_;   // breadcrumb: every cell walked through

    // ---- mines and keys (the game layer) ----
    // The layout is deterministic per (maze, seed): same maze -> same field.
    LinkedList<int> mines_;         // bomb cells (visible on the canvas)
    LinkedList<int> keysPlaced_;    // full key layout, fixed at build time
    LinkedList<int> keysLeft_;      // keys not yet collected (canvas + gameplay)
    int keysTotal_;
    int keysCollected_;
    bool dead_;                     // just hit a mine: run over until retry

    // ---- provenance of the current maze (save file / library key) ----
    unsigned int activeSeed_;
    int activeExtra_;

    // helpers
    void stopAnimation();
    void announce(const QString& text);
    MazeRecord recordOfCurrentMaze();
    void placeKeysAndMines(unsigned int seed);   // mines + keys, reachability-checked
    bool reachableAvoidingMines(int target);     // BFS with mines as blocked cells
    static int starsFor(int collected, int total);
};
