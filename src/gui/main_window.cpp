#include "main_window.h"
#include "maze_canvas.h"
#include "maze_io.h"
#include "rng.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QFileDialog>

MainWindow::MainWindow()
    : canvas_(0)
    , playhead_(0)
    , phase_(0)
    , stepsPerTick_(1)
    , visitedCount_(0)
    , routeCount_(0)
    , routeFound_(false)
    , playMode_(false)
    , won_(false)
    , playerVertex_(-1)
    , moves_(0)
    , keysTotal_(0)
    , keysCollected_(0)
    , dead_(false)
    , activeSeed_(0)
    , activeExtra_(0)
{
    // ---- widgets ----
    canvas_ = new MazeCanvas(this);

    rowsInput_ = new QSpinBox(this);
    rowsInput_->setRange(1, MazeMatrix::MAX_ROWS);
    rowsInput_->setValue(15);
    colsInput_ = new QSpinBox(this);
    colsInput_->setRange(1, MazeMatrix::MAX_COLS);
    colsInput_->setValue(15);
    seedInput_ = new QSpinBox(this);
    seedInput_->setRange(0, 2147483647);
    seedInput_->setValue(42);
    extraOpeningsInput_ = new QSpinBox(this);
    extraOpeningsInput_->setRange(0, 400);
    extraOpeningsInput_->setValue(8);
    librarySeedInput_ = new QSpinBox(this);
    librarySeedInput_->setRange(0, 2147483647);
    librarySeedInput_->setValue(42);

    generateButton_ = new QPushButton("Generate maze", this);
    solveButton_ = new QPushButton("Solve (BFS)", this);
    playButton_ = new QPushButton("Play manually (arrows/WASD)", this);
    saveButton_ = new QPushButton("Save to file...", this);
    loadButton_ = new QPushButton("Load from file...", this);
    insertButton_ = new QPushButton("Add this maze to library", this);
    searchButton_ = new QPushButton("Look up by seed", this);

    status_ = new QLabel("Set rows/cols, seed and extra openings, then Generate.", this);
    status_->setWordWrap(true);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::onFrame);
    connect(canvas_, &MazeCanvas::moveRequested, this, &MainWindow::onMoveRequested);

    connect(generateButton_, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(solveButton_, &QPushButton::clicked, this, &MainWindow::onSolve);
    connect(playButton_, &QPushButton::clicked, this, &MainWindow::onPlay);
    connect(saveButton_, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(insertButton_, &QPushButton::clicked, this, &MainWindow::onInsertInLibrary);
    connect(searchButton_, &QPushButton::clicked, this, &MainWindow::onSearchLibrary);

    // ---- layout ----
    QFormLayout* form = new QFormLayout;
    form->addRow("Rows (1-50):", rowsInput_);
    form->addRow("Cols (1-50):", colsInput_);
    form->addRow("Random seed:", seedInput_);
    form->addRow("Extra openings:", extraOpeningsInput_);

    QVBoxLayout* side = new QVBoxLayout;
    side->addLayout(form);
    side->addWidget(generateButton_);
    side->addWidget(solveButton_);
    side->addWidget(playButton_);
    side->addSpacing(10);
    side->addWidget(saveButton_);
    side->addWidget(loadButton_);
    side->addSpacing(10);
    side->addWidget(new QLabel("Library lookup seed:", this));
    side->addWidget(librarySeedInput_);
    side->addWidget(insertButton_);
    side->addWidget(searchButton_);
    side->addSpacing(10);
    side->addWidget(status_);
    side->addStretch(1);

    QHBoxLayout* top = new QHBoxLayout;
    top->addWidget(canvas_, 1);
    top->addLayout(side, 0);
    setLayout(top);

    // ---- initial state: a closed 15x15 grid is visible ----
    maze_.reset(rowsInput_->value(), colsInput_->value());
    canvas_->setMaze(&maze_);
}

void MainWindow::stopAnimation()
{
    timer_->stop();
    phase_ = 0;
    playhead_ = 0;
    visitedCount_ = 0;
    routeCount_ = 0;
    visited_.clear();
    route_.clear();
    playMode_ = false;
    won_ = false;
    playerVertex_ = -1;
    moves_ = 0;
    playerTrail_.clear();
    dead_ = false;
    canvas_->setPlayer(-1);
    canvas_->setTrail(0);
    canvas_->setBlockedWall(-1, -1, -1);
    canvas_->setExplosion(-1);
    canvas_->resetOverlays();
}

void MainWindow::announce(const QString& text)
{
    status_->setText(text);
}

MazeRecord MainWindow::recordOfCurrentMaze()
{
    MazeRecord record;
    record.seed = activeSeed_;
    record.rows = maze_.rows();
    record.cols = maze_.cols();
    record.extraPassages = activeExtra_;
    record.walls = maze_.toWallString();
    return record;
}

// Star rating exactly per the spec: all keys -> 3, half -> 2, a quarter -> 1.
int MainWindow::starsFor(int collected, int total)
{
    if (total <= 0 || collected >= total)
        return 3;
    if (collected * 2 >= total)
        return 2;
    if (collected * 4 >= total)
        return 1;
    return 0;
}

// Is 'target' reachable from the entrance when every mine is impassable?
bool MainWindow::reachableAvoidingMines(int target)
{
    IntLinkedList scratch;
    return solver_.solve(graph_, 0, target, scratch, mines_.size() ? &mines_ : 0);
}

// Seed-deterministic game field: bombs first (each rejected if it would bury
// a key or cut the maze off), then keys (each must be reachable avoiding all
// mines). Same (maze, seed) -> identical layout on any machine.
void MainWindow::placeKeysAndMines(unsigned int seed)
{
    mines_.clear();
    keysPlaced_.clear();
    keysLeft_.clear();
    keysTotal_ = 0;
    keysCollected_ = 0;
    dead_ = false;
    canvas_->setExplosion(-1);
    canvas_->setMines(&mines_);
    canvas_->setKeys(&keysLeft_);

    const int total = maze_.rows() * maze_.cols();
    if (total < 16)
        return;   // tiny mazes (incl. 1x1) carry no items

    RandomLCG rng(seed ^ 2463534242u);   // decorrelated from the carving stream

    int mineTarget = total / 14;
    if (mineTarget < 2) mineTarget = 2;
    if (mineTarget > 40) mineTarget = 40;
    int keyTarget = total / 8;
    if (keyTarget < 3) keyTarget = 3;
    if (keyTarget > 30) keyTarget = 30;

    // 1) mines: never on entrance (0) or exit (total-1); each candidate must
    //    keep the exit AND every placed key reachable
    for (int placed = 0, attempts = 0; placed < mineTarget && attempts < mineTarget * 20; ++attempts) {
        const int v = rng.nextInt(1, total - 2);
        if (mines_.contains(v) || keysPlaced_.contains(v))
            continue;
        mines_.pushBack(v);
        bool ok = reachableAvoidingMines(total - 1);
        if (ok) {
            IntLinkedList::Cursor it = keysPlaced_.cursor();
            while (ok && it.hasNext())
                ok = reachableAvoidingMines(it.next());
        }
        if (ok)
            ++placed;
        else
            mines_.removeValue(v);   // would strangle a route: not fair, reject
    }

    // 2) keys: distinct cells, each reachable from the entrance avoiding mines
    for (int placed = 0, attempts = 0; placed < keyTarget && attempts < keyTarget * 20; ++attempts) {
        const int v = rng.nextInt(1, total - 2);
        if (mines_.contains(v) || keysPlaced_.contains(v))
            continue;
        keysPlaced_.pushBack(v);
        if (reachableAvoidingMines(v))
            ++keysTotal_;
        else
            keysPlaced_.removeValue(v);
    }
    keysLeft_ = keysPlaced_;   // full display until the first play attempt
}

void MainWindow::onFrame()
{
    if (phase_ == 1) {
        // generation replay: apply recorded carves to the blank copy
        int left = stepsPerTick_;
        while (left-- > 0 && genCursor_.hasNext()) {
            const CarveStep& s = genCursor_.next();
            animMatrix_.removeWall(s.row, s.col, s.side);
            canvas_->setGenerationCursor(s.row, s.col);
            ++playhead_;
        }
        if (!genCursor_.hasNext()) {
            timer_->stop();
            canvas_->setMaze(&maze_);   // animMatrix_ + entrance/exit openings
            canvas_->setGenerationCursor(-1, -1);
            phase_ = 0;
            announce(QString("Maze ready (seed %1, %2 extra opening(s)): "
                             "%3 key(s) and %4 bomb(s) are on the field. "
                             "Play manually or press Solve.")
                     .arg(activeSeed_).arg(activeExtra_)
                     .arg(keysTotal_).arg(mines_.size()));
        }
    } else if (phase_ == 2) {
        // BFS replay: tint dequeued vertices, probe the checked edge
        int left = stepsPerTick_;
        while (left-- > 0 && solveCursor_.hasNext()) {
            const SolveStep& s = solveCursor_.next();
            if (s.kind == SolveStep::Dequeued) {
                visited_.pushBack(s.vertex);
                ++visitedCount_;
            } else if (s.kind == SolveStep::EdgeChecked) {
                canvas_->setCurrentEdge(s.vertex, s.neighbor);
            }
            ++playhead_;
        }
        canvas_->setVisited(&visited_, visitedCount_);
        if (!solveCursor_.hasNext()) {
            timer_->stop();
            canvas_->setCurrentEdge(-1, -1);
            if (!routeFound_) {
                phase_ = 0;
                announce("No route from entrance to exit — BFS drained its queue.");
            } else {
                phase_ = 3;
                routeCount_ = 0;
                timer_->start(20);
            }
        }
    } else if (phase_ == 3) {
        // route replay: reveal the shortest route entrance -> exit
        routeCount_ += stepsPerTick_;
        if (routeCount_ > route_.size())
            routeCount_ = route_.size();
        canvas_->setRoute(&route_, routeCount_);
        if (routeCount_ >= route_.size()) {
            timer_->stop();
            phase_ = 0;
            announce(QString("Shortest route: %1 cell(s); BFS replayed %2 step(s).")
                     .arg(route_.size()).arg(solveSteps_.size()));
        }
    }
}

void MainWindow::onGenerate()
{
    stopAnimation();
    if (!maze_.reset(rowsInput_->value(), colsInput_->value())) {
        announce("Invalid maze size.");
        return;
    }
    activeSeed_ = (unsigned int)seedInput_->value();
    activeExtra_ = extraOpeningsInput_->value();

    MazeGenerator generator(maze_, activeSeed_);
    generator.generate();
    if (activeExtra_ > 0)
        generator.openExtraPassages(activeExtra_);
    genSteps_ = generator.steps();          // copy of the recorded carve sequence
    graph_.buildFromMatrix(maze_);
    placeKeysAndMines(activeSeed_);         // bombs + keys laid out for this maze

    animMatrix_.reset(maze_.rows(), maze_.cols());   // all-wall blank for replay
    canvas_->setMaze(&animMatrix_);
    genCursor_ = genSteps_.cursor();
    playhead_ = 0;
    phase_ = 1;
    stepsPerTick_ = 1 + genSteps_.size() / 200;      // ~4 s at every maze size
    timer_->start(20);
    announce(QString("Carving a %1 x %2 maze (seed %3)...")
             .arg(rowsInput_->value()).arg(colsInput_->value()).arg(activeSeed_));
}

void MainWindow::onSolve()
{
    if (graph_.vertexCount() == 0) {
        announce("Generate or load a maze first.");
        return;
    }
    stopAnimation();
    canvas_->setMaze(&maze_);   // BFS never modifies the maze

    int target = graph_.vertexCount() - 1;   // entrance = vertex 0, exit = last
    // BFS too respects the mines: the overlay route is the bomb-safe optimum
    routeFound_ = solver_.solve(graph_, 0, target, route_, mines_.size() ? &mines_ : 0);
    if (!routeFound_ && mines_.size() > 0)   // defensive: placement should guarantee it
        routeFound_ = solver_.solve(graph_, 0, target, route_);
    solveSteps_ = solver_.steps();
    solveCursor_ = solveSteps_.cursor();

    playhead_ = 0;
    visited_.clear();
    visitedCount_ = 0;
    phase_ = 2;
    stepsPerTick_ = 1 + solveSteps_.size() / 200;
    timer_->start(20);
    announce(routeFound_ ? QString("BFS searching (%1 recorded steps)...")
                         .arg(solveSteps_.size())
                         : QString("BFS searching... (queue may drain: no route)"));
}

void MainWindow::onPlay()
{
    if (graph_.vertexCount() == 0) {
        announce("Generate or load a maze first.");
        return;
    }
    stopAnimation();
    canvas_->setMaze(&maze_);      // show the real maze, not the replay copy

    playMode_ = true;
    won_ = false;
    dead_ = false;
    moves_ = 0;
    playerVertex_ = 0;             // the pawn starts at the entrance (top-left)
    playerTrail_.clear();
    playerTrail_.pushBack(0);
    keysLeft_ = keysPlaced_;       // fresh attempt: every key back on the field
    keysCollected_ = 0;
    canvas_->setPlayer(0);
    canvas_->setTrail(&playerTrail_);
    canvas_->setExplosion(-1);
    canvas_->setFocus();           // the canvas takes the keyboard

    if (graph_.vertexCount() == 1) {
        won_ = true;               // 1x1: entrance IS the exit
        announce("1x1 maze: the entrance IS the exit — 3 stars, obviously!");
        return;
    }
    announce(QString("Your turn! Click the maze, then walk with ARROW KEYS or "
                     "WASD. Walls block you (red flash). Collect keys (%1 on "
                     "the field), dodge the %2 pixel bombs — stepping on one "
                     "is game over. Exit: blue notch bottom-right.")
                 .arg(keysTotal_).arg(mines_.size()));
}

void MainWindow::onMoveRequested(int side)
{
    if (!playMode_ || won_ || dead_)
        return;                    // keys only move the pawn while alive in play mode

    const int cols = maze_.cols();
    const int rows = maze_.rows();
    const int r = playerVertex_ / cols;
    const int c = playerVertex_ % cols;

    int drow;
    int dcol;
    MazeMatrix::sideDelta(side, drow, dcol);
    const int nr = r + drow;
    const int nc = c + dcol;

    if (maze_.hasWall(r, c, side) || nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
        // bumped into a wall (or the outside): flash it red and stay put
        canvas_->setBlockedWall(r, c, side);
        QTimer::singleShot(250, this, [this]() { canvas_->setBlockedWall(-1, -1, -1); });
        announce(QString("Blocked by a wall — find another way. (%1 move(s) so far)")
                     .arg(moves_));
        return;
    }

    playerVertex_ = nr * cols + nc;
    ++moves_;
    playerTrail_.pushBack(playerVertex_);
    if (playerTrail_.size() > 5000) { // wandering forever? keep it bounded
        int dropped;                  // (oldest breadcrumb quietly drops)
        playerTrail_.popFront(dropped);
    }
    canvas_->setPlayer(playerVertex_);
    canvas_->setTrail(&playerTrail_);
    canvas_->setBlockedWall(-1, -1, -1);

    // stepped on a mine? -> instant game over
    if (mines_.contains(playerVertex_)) {
        dead_ = true;
        canvas_->setExplosion(playerVertex_);
        const int stars = starsFor(keysCollected_, keysTotal_);
        announce(QString("BOOM! You walked onto a pixel bomb with %1/%2 key(s) "
                         "in %3 move(s) — Stars: %4 of 3. Press Play again to retry.")
                     .arg(keysCollected_).arg(keysTotal_).arg(moves_).arg(stars));
        return;
    }

    // stepped on a key? -> collect it
    const bool keyGot = keysLeft_.removeValue(playerVertex_);
    if (keyGot)
        ++keysCollected_;

    const int exitVertex = rows * cols - 1;
    if (playerVertex_ == exitVertex) {
        won_ = true;   // payoff: overlay the bomb-safe BFS optimum
        route_.clear();
        solver_.solve(graph_, 0, exitVertex, route_, mines_.size() ? &mines_ : 0);
        if (route_.isEmpty())
            solver_.solve(graph_, 0, exitVertex, route_);
        if (route_.isEmpty())     // degenerate single-cell walk
            route_.pushBack(playerVertex_);
        canvas_->setRoute(&route_, route_.size());

        const int stars = starsFor(keysCollected_, keysTotal_);
        announce(QString("You reached the exit in %1 move(s), %2/%3 keys — "
                         "Stars: %4 of 3  (BFS bomb-safe optimum: %5 cells, red).")
                     .arg(moves_).arg(keysCollected_).arg(keysTotal_)
                     .arg(stars).arg(route_.size()));
    } else if (keyGot) {
        announce(QString("Key collected! %1/%2 — %3 move(s).")
                     .arg(keysCollected_).arg(keysTotal_).arg(moves_));
    } else {
        announce(QString("Cell (%1,%2) — %3 move(s). Keys so far: %4/%5.")
                     .arg(nr).arg(nc).arg(moves_).arg(keysCollected_).arg(keysTotal_));
    }
}

void MainWindow::onSave()
{
    QString path = QFileDialog::getSaveFileName(this, "Save maze", QString(),
                                                "Maze text files (*.txt)");
    if (path.isEmpty())
        return;
    std::string error;
    if (MazeIO::saveToFile(path.toStdString(), maze_, activeSeed_, activeExtra_, error))
        announce(QString("Saved: %1 (seed %2)").arg(path).arg(activeSeed_));
    else
        announce(QString("Save failed: %1").arg(QString::fromStdString(error)));
}

void MainWindow::onLoad()
{
    QString path = QFileDialog::getOpenFileName(this, "Load maze", QString(),
                                                "Maze text files (*.txt);;All files (*.*)");
    if (path.isEmpty())
        return;
    LoadResult result = MazeIO::loadFromFile(path.toStdString());
    if (!result.ok) {
        announce(QString("Load failed: %1 — current maze untouched.")
                 .arg(QString::fromStdString(result.error)));
        return;
    }
    stopAnimation();                          // fail-atomic swap-in
    maze_ = result.maze;
    activeSeed_ = result.seed;
    activeExtra_ = result.extraPassages;
    graph_.buildFromMatrix(maze_);
    placeKeysAndMines(activeSeed_);   // deterministic bombs/keys for this maze
    canvas_->setMaze(&maze_);
    rowsInput_->setValue(maze_.rows());
    colsInput_->setValue(maze_.cols());
    announce(QString("Loaded %1 x %2 maze (seed %3) — %4 key(s), %5 bomb(s) on the field.")
             .arg(maze_.rows()).arg(maze_.cols()).arg(activeSeed_)
             .arg(keysTotal_).arg(mines_.size()));
}

void MainWindow::onInsertInLibrary()
{
    if (maze_.inBounds(0, 0)) {
        library_.insert(recordOfCurrentMaze());
        announce(QString("In library: seed %1 (tree holds %2 maze(s) now).")
                 .arg(activeSeed_).arg(library_.size()));
    }
}

void MainWindow::onSearchLibrary()
{
    MazeRecord found;
    unsigned int query = (unsigned int)librarySeedInput_->value();
    if (library_.findBySeed(query, found)) {
        // only trusted saved lives here; insertions come from valid matrices
        stopAnimation();
        maze_ = MazeMatrix::fromWallString(found.rows, found.cols, found.walls);
        activeSeed_ = found.seed;
        activeExtra_ = found.extraPassages;
        graph_.buildFromMatrix(maze_);
        placeKeysAndMines(activeSeed_);   // same seed -> same bombs and keys
        canvas_->setMaze(&maze_);
        rowsInput_->setValue(maze_.rows());
        colsInput_->setValue(maze_.cols());
        announce(QString("Seed %1 found in library — maze restored "
                         "(%2 x %3, %4 key(s), %5 bomb(s)).")
                 .arg(found.rows).arg(found.cols)
                 .arg(keysTotal_).arg(mines_.size()).arg(query));
    } else {
        announce(QString("Seed %1 is not in the library.").arg(query));
    }
}
