#pragma once
// MainWindow — the Qt layer of the project. It owns the core objects as
// members (no manual new/delete anywhere), drives them once per button click,
// and replays the recorded step lists with a 20 ms QTimer.
//
// Replay model:
//   phase 1: generation — CarveSteps are applied to animMatrix_ (a blank copy),
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
    QSpinBox* rowsInput_;          // user picks maze height here
    QSpinBox* colsInput_;          // user picks maze width here
    QSpinBox* seedInput_;          // user picks the generation seed here
    QSpinBox* extraOpeningsInput_; // user picks how many extra openings to add
    QSpinBox* librarySeedInput_;   // seed typed for the library lookup
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
    CarveStepList genSteps_;               // recorded carving, replayed in phase 1
    SolveStepList solveSteps_;             // recorded BFS actions, replayed in phase 2
    CarveStepList::Cursor genCursor_;      // walk genSteps_ during phase 1
    SolveStepList::Cursor solveCursor_;    // walk solveSteps_ during phase 2
    IntLinkedList visited_;                // dequeued order, grows during replay
    IntLinkedList route_;                  // final BFS route, replayed in phase 3
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
    IntLinkedList playerTrail_;   // breadcrumb: every cell walked through

    // ---- mines and keys (the game layer) ----
    // The layout is deterministic per (maze, seed): same maze -> same field.
    IntLinkedList mines_;         // bomb cells (visible on the canvas)
    IntLinkedList keysPlaced_;    // full key layout, fixed at build time
    IntLinkedList keysLeft_;      // keys not yet collected (canvas + gameplay)
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
