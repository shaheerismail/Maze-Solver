#include "main_window.h"
#include "maze_canvas.h"
#include "maze_io.h"

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
    , activeSeed_(0)
    , activeExtra_(0)
{
    // ---- widgets ----
    canvas_ = new MazeCanvas(this);

    rows_ = new QSpinBox(this);
    rows_->setRange(1, MazeMatrix::MAX_ROWS);
    rows_->setValue(15);
    cols_ = new QSpinBox(this);
    cols_->setRange(1, MazeMatrix::MAX_COLS);
    cols_->setValue(15);
    seed_ = new QSpinBox(this);
    seed_->setRange(0, 2147483647);
    seed_->setValue(42);
    extra_ = new QSpinBox(this);
    extra_->setRange(0, 400);
    extra_->setValue(8);
    librarySeed_ = new QSpinBox(this);
    librarySeed_->setRange(0, 2147483647);
    librarySeed_->setValue(42);

    generateButton_ = new QPushButton("Generate maze", this);
    solveButton_ = new QPushButton("Solve (BFS)", this);
    saveButton_ = new QPushButton("Save to file...", this);
    loadButton_ = new QPushButton("Load from file...", this);
    insertButton_ = new QPushButton("Add this maze to library", this);
    searchButton_ = new QPushButton("Look up by seed", this);

    status_ = new QLabel("Set rows/cols, seed and extra openings, then Generate.", this);
    status_->setWordWrap(true);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::onFrame);

    connect(generateButton_, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(solveButton_, &QPushButton::clicked, this, &MainWindow::onSolve);
    connect(saveButton_, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(loadButton_, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(insertButton_, &QPushButton::clicked, this, &MainWindow::onInsertInLibrary);
    connect(searchButton_, &QPushButton::clicked, this, &MainWindow::onSearchLibrary);

    // ---- layout ----
    QFormLayout* form = new QFormLayout;
    form->addRow("Rows (1-50):", rows_);
    form->addRow("Cols (1-50):", cols_);
    form->addRow("Random seed:", seed_);
    form->addRow("Extra openings:", extra_);

    QVBoxLayout* side = new QVBoxLayout;
    side->addLayout(form);
    side->addWidget(generateButton_);
    side->addWidget(solveButton_);
    side->addSpacing(10);
    side->addWidget(saveButton_);
    side->addWidget(loadButton_);
    side->addSpacing(10);
    side->addWidget(new QLabel("Library lookup seed:", this));
    side->addWidget(librarySeed_);
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
    maze_.reset(rows_->value(), cols_->value());
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

void MainWindow::onFrame()
{
    if (phase_ == 1) {
        // generation replay: apply recorded carves to the blank copy
        int left = stepsPerTick_;
        while (left-- > 0 && genCursor_.hasNext()) {
            const GenStep& s = genCursor_.next();
            animMatrix_.removeWall(s.row, s.col, s.side);
            canvas_->setGenerationCursor(s.row, s.col);
            ++playhead_;
        }
        if (!genCursor_.hasNext()) {
            timer_->stop();
            canvas_->setMaze(&maze_);   // animMatrix_ + entrance/exit openings
            canvas_->setGenerationCursor(-1, -1);
            phase_ = 0;
            announce(QString("Maze ready (seed %1, %2 extra opening(s))."
                             " Now press Solve.")
                     .arg(activeSeed_).arg(activeExtra_));
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
    if (!maze_.reset(rows_->value(), cols_->value())) {
        announce("Invalid maze size.");
        return;
    }
    activeSeed_ = (unsigned int)seed_->value();
    activeExtra_ = extra_->value();

    MazeGenerator generator(maze_, activeSeed_);
    generator.generate();
    if (activeExtra_ > 0)
        generator.openExtraPassages(activeExtra_);
    genSteps_ = generator.steps();          // copy of the recorded carve sequence
    graph_.buildFromMatrix(maze_);

    animMatrix_.reset(maze_.rows(), maze_.cols());   // all-wall blank for replay
    canvas_->setMaze(&animMatrix_);
    genCursor_ = genSteps_.iterator();
    playhead_ = 0;
    phase_ = 1;
    stepsPerTick_ = 1 + genSteps_.size() / 200;      // ~4 s at every maze size
    timer_->start(20);
    announce(QString("Carving a %1 x %2 maze (seed %3)...")
             .arg(rows_->value()).arg(cols_->value()).arg(activeSeed_));
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
    routeFound_ = solver_.solve(graph_, 0, target, route_);
    solveSteps_ = solver_.steps();
    solveCursor_ = solveSteps_.iterator();

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
    canvas_->setMaze(&maze_);
    rows_->setValue(maze_.rows());
    cols_->setValue(maze_.cols());
    announce(QString("Loaded %1 x %2 maze (seed %3).")
             .arg(maze_.rows()).arg(maze_.cols()).arg(activeSeed_));
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
    unsigned int query = (unsigned int)librarySeed_->value();
    if (library_.findBySeed(query, found)) {
        // only trusted saved lives here; insertions come from valid matrices
        stopAnimation();
        maze_ = MazeMatrix::fromWallString(found.rows, found.cols, found.walls);
        activeSeed_ = found.seed;
        activeExtra_ = found.extraPassages;
        graph_.buildFromMatrix(maze_);
        canvas_->setMaze(&maze_);
        rows_->setValue(maze_.rows());
        cols_->setValue(maze_.cols());
        announce(QString("Seed %1 found in library — maze restored "
                         "(%2 x %3, %4 extra opening(s)).")
                 .arg(query).arg(found.rows).arg(found.cols).arg(found.extraPassages));
    } else {
        announce(QString("Seed %1 is not in the library.").arg(query));
    }
}
