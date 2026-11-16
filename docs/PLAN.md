# Maze Solver — Implementation Plan

> **Staging note:** this document is the deliverable. It is staged here only because Plan mode cannot write files outside the harness plan directory. On switching to Build mode, the first action is to copy this file **unmodified** to `Maze-Solver/docs/PLAN.md` (inside the user's `Desktop\DSA MAZE GAME` folder) and delete this staging copy. No other files are created.

Project: **Maze Solver** (Data Structures course project, Qt 6 Widgets, C++17, single developer)
Repo root: `Maze-Solver/` (currently contains `README.md` and `.git`; no source yet). All paths below are relative to that root.

---

## Assumptions

Stated up front because they are decisions, not open questions. Flag any of them if wrong.

1. **No external APIs.** "No APIs" is read as: nothing beyond (a) the plain C++ language, (b) the standard-library facilities already approved for this project — `std::string` plus `std::ifstream`/`std::ofstream`/`std::getline` for text file I/O, and (c) Qt Widgets for the window, QPainter drawing, `QTimer` and `QFileDialog` (the course spec and the README both require the Qt GUI). Everything else is hand-written: no `strtol`/`atoi`/`sscanf` (custom integer parser), no `<chrono>` (no timing assertions in tests), no `<random>` (hand-rolled LCG), no exceptions (return codes + error strings), no `<cassert>` for input handling (one explicit validation point in `MazeIO`). If Qt itself is also banned and there is no GUI requirement, say so — the animated window would be dropped and this becomes a console-only maze, a much smaller plan.
2. **Rectangular maze** with independent rows and cols (README says "adjustable size"; two spin boxes beat one). Default 15×15.
3. **Size cap 50×50** (`MAX_ROWS = MAX_COLS = 50`). Worst-case recursion depth = 2500 frames ≈ 0.3 MB on MSVC — safe on the default 1 MB stack (see §9).
4. **Entrance/exit convention:** entrance = north wall of cell (0,0); exit = south wall of cell (rows−1, cols−1). They are *walls in the matrix* (a cleared outside wall), not separate vertex types. BFS source = (0,0), target = (rows−1, cols−1).
5. **RNG:** hand-rolled LCG class (`RandomLCG`) in core. `<random>` (`std::mt19937`) is off-limits under the no-API rule anyway, and a plain LCG recurrence is the easiest thing to explain line-by-line in a viva.
6. **Seed type:** `unsigned int` in core; GUI spin box limited to 0..2,147,483,647 (QSpinBox is int-based). Noted as a cosmetic limit.
7. **Qt messages:** status text goes in the window's status bar (`QLabel`), not `QMessageBox`, to stay strictly within the allowed Qt usage list (window, drawing, timers, file dialogs). File dialogs use `QFileDialog` only, and the GUI converts `QString ⇄ std::string` only in `MainWindow` — core never sees a `QString`.
8. **Testing framework:** hand-rolled macro-based test harness in `tests/` (no Catch2/doctest) — zero new dependencies, trivially explainable, and works offline.
9. **BST duplicate policy:** re-saving a maze with an existing seed **replaces** the stored maze (seed is the key; the tree never stores duplicates).
10. **Library persistence is not in scope** — the BST is an in-memory library; mazes survive between sessions via Save/Load text files. (Alternative: serialize the whole BST to `library.txt` on exit; rejected as extra scope.)
11. CMake + the Qt kit are *build tooling*, not project code — CMakeLists/presets configure how the hand-written C++ compiles; they add no library calls into the program.
12. The repo subfolder `Maze-Solver` is the project root for CMake (it holds `README.md` and `.git`); the parent desktop folder is ignored.

---

## 1. Architecture overview

Two strictly separated layers:

- **`maze_core`** — every data structure and algorithm. Depends only on the C++ language and the approved basics (`std::string`, file streams). **Contains no Qt header, no third-party code, no utility-library calls** — all parsing, randomness and error signalling below is hand-written — so it builds and tests headless and every line is explainable in the viva.
- **`maze_gui`** — Qt Widgets only. Builds/core objects as members, calls core APIs, replays recorded steps with a `QTimer`, writes text files through `MazeIO` (paths come from `QFileDialog`), and paints whatever the core currently holds. Core never calls back into Qt; the flow is one-way: GUI drives, core computes, GUI draws.

Communication pattern: **record-and-replay**. Long operations (generation = ~V−1 carve steps; BFS = dequeues + edge checks) are run once, synchronously, and record their actions into custom linked lists of step structs. The GUI then replays those recorded steps against a copy of the matrix using a 20 ms `QTimer` — no blocking, no core→Qt callbacks.

```
+------------------------------ Qt layer (gui/) -------------------------------+
|  MainWindow (QWidget)                                                        |
|    controls: QSpinBox rows/cols/seed/extra, buttons, QLineEdit search        |
|    QTimer  -- replay tick (20 ms) --> apply next step, canvas->update()      |
|    QFileDialog -> paths only  |  QString<->std::string at this boundary only  |
|  MazeCanvas (QWidget)                                                        |
|    paintEvent(): QPainter draws walls, BFS colors, route, generation cursor  |
+-------------+----------------------------------------------------+----------+
              | calls core API, owns core objects as members       |
              | (values, never manual new/delete in the GUI)       |
              v                                                    v
+--------------------------- maze_core (core/) — Qt-free ----------------------+
|  MazeMatrix  Cell[walls:4bit][visited]   + toAscii() / toWallString() etc.   |
|  MazeGenerator  recursive backtracker -> CarveStepList steps                |
|                 + openEntranceExit() + openRandomPassages(count)            |
|  MazeGraph    adjacency lists: IntLinkedList*                              |
|  BfsSolver    IntLinkedQueue + parent[] -> IntLinkedList route + SolveStepList |
|  MazeLibrary  MazeRecord BST keyed by seed                                 |
|  MazeIO       text save/load + validation (hand-written parser)            |
|  primitives: IntLinkedList, IntLinkedQueue, RandomLCG  (standalone DSA,     |
|              each with its own .h/.cpp — no template classes)               |
+-------------------------------------------------------------------------------+
        | std::fstream                                    | hex wall string
        v                                                 v
   MazeSolve_v1 text files                    MazeRecord{seed,rows,cols,extra,walls}
```

---

## 2. Folder and file layout + CMake outline

```
Maze-Solver/
├─ CMakeLists.txt
├─ CMakePresets.json              # pins generator + CMAKE_PREFIX_PATH to the Qt kit
├─ .gitignore
├─ README.md                      # (exists)
├─ docs/
│  └─ PLAN.md                     # this plan
├─ src/
│  ├─ core/                       # Qt-free; every class has its own .h/.cpp
│  │  ├─ maze_matrix.h / .cpp     # Cell, MazeMatrix
│  │  ├─ rng.h / .cpp             # RandomLCG
│  │  ├─ int_linked_list.h / .cpp # IntLinkedList (standalone, no templates)
│  │  ├─ int_linked_queue.h / .cpp# IntLinkedQueue (standalone, no templates)
│  │  ├─ steps.h                  # struct CarveStep, struct SolveStep
│  │  ├─ carve_step_list.h / .cpp # CarveStepList (standalone, no templates)
│  │  ├─ solve_step_list.h / .cpp # SolveStepList (standalone, no templates)
│  │  ├─ maze_record_list.h / .cpp# MazeRecordList (standalone, no templates)
│  │  ├─ string_list.h / .cpp     # StringList (standalone, no templates)
│  │  ├─ maze_generator.h / .cpp
│  │  ├─ maze_graph.h / .cpp
│  │  ├─ bfs_solver.h / .cpp
│  │  ├─ maze_library.h / .cpp    # BST
│  │  ├─ maze_io.h / .cpp
│  │  └─ CMakeLists.txt
│  └─ gui/
│     ├─ main.cpp                 # QApplication entry
│     ├─ main_window.h / .cpp     # controls, QTimer replay, file dialogs
│     ├─ maze_canvas.h / .cpp     # custom paintEvent widget
│     └─ CMakeLists.txt
└─ tests/
   ├─ test_util.h                 # CHECK macros + counters
   ├─ test_main.cpp               # calls each test suite; also --demo ASCII gallery
   ├─ test_matrix.cpp
   ├─ test_lists.cpp
   ├─ test_generator.cpp
   ├─ test_graph_bfs.cpp
   ├─ test_extra.cpp
   ├─ test_io.cpp
   ├─ test_library.cpp
   └─ CMakeLists.txt
```

`CMakeLists.txt` (root) — outline:

```cmake
cmake_minimum_required(VERSION 3.19)
project(MazeSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/core)    # target maze_core   (STATIC)

add_subdirectory(tests)       # target maze_tests  (console executable)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)         # alternative: qt_standard_project_setup() on Qt >= 6.3
add_subdirectory(src/gui)     # target maze_gui    (WIN32 executable) -> Qt6::Widgets + maze_core
```

`src/core/CMakeLists.txt`:

```cmake
add_library(maze_core STATIC
    maze_matrix.cpp rng.cpp maze_generator.cpp maze_graph.cpp
    bfs_solver.cpp maze_library.cpp maze_io.cpp)
target_include_directories(maze_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(maze_core PUBLIC cxx_std_17)
```

`src/gui/CMakeLists.txt`:

```cmake
add_executable(maze_gui WIN32 main.cpp main_window.cpp maze_canvas.cpp)
target_link_libraries(maze_gui PRIVATE maze_core Qt6::Widgets)
```

`tests/CMakeLists.txt`:

```cmake
add_executable(maze_tests
    test_main.cpp test_matrix.cpp test_lists.cpp test_generator.cpp
    test_graph_bfs.cpp test_extra.cpp test_io.cpp test_library.cpp)
target_link_libraries(maze_tests PRIVATE maze_core)
```

Reasons: three targets exactly as required (core static lib / console tests / GUI exe). `maze_tests --demo` doubles as the console demo (prints an ASCII maze + solved route to stdout) — one extra flag instead of a fourth target; alternative is a separate `mazeCli` target, dropped to keep the viva story simple. `WIN32` hides the console window for the GUI app.

`CMakePresets.json` (VS Code picks this up; adjust the Qt path/kit to what is installed):

```json
{
  "version": 3,
  "configurePresets": [{
    "name": "windows-qt",
    "generator": "Visual Studio 17 2022",
    "binaryDir": "${sourceDir}/build",
    "cacheVariables": { "CMAKE_PREFIX_PATH": "C:/Qt/<version>/<kit>" }
  }]
}
```

---

## 3. Class and API design (signatures only)

Wall encoding shared by everyone: bits `N=1, E=2, S=4, W=8`; bit set = wall present. Cell visual: hex digit `0..F` in files, 2-D nibble in memory.

### `steps.h` (plain structs, no Qt)

```cpp
struct CarveStep { enum Kind { Carve, ExtraOpening } kind; int row, col, side; };   // wall opened at (row,col)
struct SolveStep {
    enum Kind { Dequeued, EdgeChecked, Found } kind;
    int vertex;        // dequeued vertex, or vertex whose edge is checked
    int neighbor;      // valid when kind == EdgeChecked
    bool discovered;   // whether this edge enqueued a new vertex
};
struct MazeRecord {
    unsigned int seed; int rows, cols, extraPassages;
    std::string walls;             // rows*cols hex chars, row-major (see §6)
};
```

### `maze_matrix.h` — the 2-D matrix

```cpp
struct Cell { unsigned char walls; bool visited; };   // walls bitmask 1|2|4|8

class MazeMatrix {
public:
    static const int WALL_N = 1, WALL_E = 2, WALL_S = 4, WALL_W = 8;
    static const int MAX_ROWS = 50, MAX_COLS = 50;

    MazeMatrix();                                   // empty 0x0 matrix
    MazeMatrix(const MazeMatrix& other);            // deep copy
    MazeMatrix& operator=(const MazeMatrix& other); // deep copy (copy-and-swap)
    ~MazeMatrix();

    static bool isValidSize(int rows, int cols);    // the ONLY size rule: 1..MAX_ROWS / 1..MAX_COLS

    bool reset(int rows, int cols);                 // reallocate closed grid; false (unchanged) on invalid size
    int rows() const;
    int cols() const;
    bool inBounds(int row, int col) const;
    bool hasWall(int row, int col, int side) const;   // side in {WALL_N, WALL_E, WALL_S, WALL_W}
    void removeWall(int row, int col, int side);      // also clears the neighbour's opposite bit
    void openOutside(int row, int col, int side);     // clears one bit only — entrance/exit, no neighbour
    void setVisited(int row, int col, bool value);
    bool isVisited(int row, int col) const;
    void clearVisited();                            // for re-running generation

    std::string toAscii() const;                    // '+' '-' '|' art, for console tests / demo
    std::string toWallString() const;               // rows*cols hex chars, row-major
    static MazeMatrix fromWallString(int rows, int cols, const std::string& hex);
    // precondition (documented, trusted): hex is already validated by MazeIO —
    // size matches and every char is a hex digit. MazeIO is the single validation point.

private:
    int rows_;
    int cols_;
    Cell** cells_;   // new Cell*[rows_]; each row new Cell[cols_]
};
```

**Ownership/memory and error rules:** the matrix is the *only* owner of `cells_`. Allocate: `new Cell*[rows_]` then `new Cell[cols_]` per row. Destructor: `delete[]` each row then the pointer array, `cells_ = nullptr` so double-delete is impossible. Copy ctor and `operator=` deep copy via copy-and-swap; assignment to self safe. **No exceptions anywhere** — the invalid-size case is handled by `reset()` returning `false` and leaving the matrix untouched (see T02); external input validity is centralized in `MazeIO`, so core methods can trust their preconditions.

### `rng.h`

```cpp
class RandomLCG {
public:
    explicit RandomLCG(unsigned int seed);
    void reseed(unsigned int seed);
    unsigned int next();                       // state = state * 1103515245 + 12345; return high 16 bits
    int nextInt(int lo, int hiInclusive);      // lo + next() % (hi - lo + 1)
private:
    unsigned int state_;                       // never zero (0 seeded -> forced to 1)
};
```

A `.cpp` file keeps the LCG recurrence in exactly one translation unit (easy to point to in a viva).

### `int_linked_list.h` / `int_linked_queue.h` (standalone DSA classes, each with its own .cpp — no templates per course rules)

```cpp
class IntLinkedList {                     // singly linked list of int cell ids
public:
    IntLinkedList();
    IntLinkedList(const IntLinkedList& other);             // deep copy
    IntLinkedList& operator=(const IntLinkedList& other);  // deep copy
    ~IntLinkedList();

    void pushBack(int value);         // O(1) via tail_
    void pushFront(int value);        // O(1) — used by BFS route reconstruction
    bool popFront(int& outValue);     // false when empty
    bool contains(int value) const;   // linear scan (mine/key membership)
    bool removeValue(int value);      // unlink the FIRST matching node
    bool isEmpty() const;
    int size() const;
    void clear();                     // deletes all nodes

    class Cursor {                    // read-only forward walk (GUI drawing)
    public:
        bool hasNext() const;
        int next();                   // returns current value, then advances
    private:
        friend class IntLinkedList;
        explicit Cursor(const Node* node);
        const Node* node_;
    };
    Cursor cursor() const;

private:
    struct Node { int value; Node* next; };
    Node* head_;
    Node* tail_;
    int size_;
};

class IntLinkedQueue {                    // FIFO queue of ints (BFS frontier)
public:
    IntLinkedQueue();
    IntLinkedQueue(const IntLinkedQueue& other);             // deep copy
    IntLinkedQueue& operator=(const IntLinkedQueue& other);  // deep copy
    ~IntLinkedQueue();
    void enqueue(int value);                            // O(1)
    bool dequeue(int& outValue);                        // O(1); false when empty
    bool isEmpty() const;
    int size() const;
    void clear();
private:
    struct Node { int value; Node* next; };
    Node* head_;   // dequeue here
    Node* tail_;   // enqueue here
    int size_;
};
```

The step/record lists follow the same standalone pattern, one class per element type, each in its own .h/.cpp pair: `CarveStepList` (nodes hold `CarveStep`), `SolveStepList` (nodes hold `SolveStep`), `MazeRecordList` (nodes hold `MazeRecord`), and `StringList` (nodes hold `std::string`, used by the save-file line splitter). No class in the project is a template.

**Ownership:** each container owns its nodes; nodes are never shared or exposed. `clear()` and destructor delete in order. Implementations live in .cpp files because these are ordinary classes — no header-only templates remain.

### `maze_generator.h`

```cpp
class MazeGenerator {
public:
    MazeGenerator(MazeMatrix& maze, unsigned int seed);  // holds maze by reference (caller-owned)
    void generate();                       // clearVisited, carve(0,0), open entrance/exit
    void openExtraPassages(int count);     // 'ExtraOpening' CarveSteps; uses the SAME rng stream as generate()
                                           // and never re-opens an already-open interior wall
    const CarveStepList& steps() const;
private:
    MazeMatrix& maze_;
    RandomLCG rng_;
    CarveStepList steps_;
    void carve(int row, int col);          // recursive backtracking body
};
```

`generate()` is fully determined by `seed`; `openExtraPassages(k)` additionally depends on `k` (same seed + same count ⇒ same maze). Direction shuffling uses Fisher–Yates over a plain local `int dirs[4]` array with `RandomLCG`. Steps are recorded so the GUI can replay the carve sequence onto a blank matrix copy.

### `maze_graph.h`

```cpp
class MazeGraph {
public:
    MazeGraph();
    MazeGraph(const MazeGraph&) = delete;   // rebuilt each time instead of deep-copied (simpler, never needed)
    MazeGraph& operator=(const MazeGraph&) = delete;
    ~MazeGraph();

    void buildFromMatrix(const MazeMatrix& maze);      // reset + one vertex per cell, edge per open passage
    int vertexCount() const;
    int rowOf(int vertex) const;                       // vertex == row * cols + col
    int colOf(int vertex) const;
    const IntLinkedList& neighbors(int vertex) const; // returns that vertex's adjacency list

private:
    int rows_;
    int cols_;
    int count_;
    IntLinkedList* adjacency_;   // new IntLinkedList[count_]; destructor uses delete[]
};
```

**Ownership:** the graph owns the array of adjacency lists; each list owns its nodes. `buildFromMatrix` deletes the old array first, so rebuild is leak-free. Edges are added by scanning each cell's E and S neighbours only — each interior passage lands in exactly one scan position and is inserted into both directions symmetrically.

### `bfs_solver.h`

```cpp
class BfsSolver {
public:
    BfsSolver();
    // Returns true iff a route exists; on success outRoute holds vertex ids entrance -> exit.
    // steps() afterwards holds the full Dequeued/EdgeChecked/Found trace for animation.
    bool solve(const MazeGraph& graph, int source, int target, IntLinkedList& outRoute,
               const IntLinkedList* blockedVertices = 0);
    const SolveStepList& steps() const;

private:
    SolveStepList steps_;
    // per-call scratch, allocated/freed inside solve():
    //   unsigned char* seen_    -> new unsigned char[graph.vertexCount()]
    //   int* parent_            -> new int[graph.vertexCount()], parent[source] = -1
};
```

**Ownership:** scratch arrays are allocated once at the top of `solve()` and `delete[]`-ed once before each of its two exits (found / no-route) — the explicit discipline is: *allocate at entry, free at every return site*. No per-call state escapes the function.

### `maze_library.h` — BST keyed by seed

```cpp
class MazeLibrary {
public:
    MazeLibrary();
    MazeLibrary(const MazeLibrary&) = delete;
    MazeLibrary& operator=(const MazeLibrary&) = delete;
    ~MazeLibrary();

    void insert(const MazeRecord& record);                    // replaces if the seed already exists
    bool findBySeed(unsigned int seed, MazeRecord& out) const;
    bool removeBySeed(unsigned int seed);                     // classic 3-case BST delete (successor for 2 children)
    int size() const;
    void inOrder(MazeRecordList& out) const;          // ascending by seed — GUI "browse library"

private:
    struct Node { MazeRecord record; Node* left; Node* right; };
    Node* root_;
    int size_;
    Node* insert(Node* node, const MazeRecord& record);       // recursive
    Node* find(Node* node, unsigned int seed) const;          // recursive
    Node* remove(Node* node, unsigned int seed, bool& removed);
    Node* minValueNode(Node* node) const;                     // in-order successor helper
    void inOrder(Node* node, MazeRecordList& out) const;
    void destroy(Node* node);                                 // post-order recursive free
};
```

**Ownership:** the tree owns all nodes and the `MazeRecord` inside them (including the `walls` string). Insert-replace frees the old record copy; `destroy` walks post-order.

### `maze_io.h`

```cpp
struct LoadResult {
    bool ok;
    std::string error;    // human-readable, includes a line number when relevant
    MazeMatrix maze;      // valid only when ok
    unsigned int seed;
    int extraPassages;
};

class MazeIO {
public:
    static bool saveToFile(const std::string& path, const MazeMatrix& maze,
                           unsigned int seed, int extraPassages, std::string& errorOut);
    static std::string saveToString(const MazeMatrix& maze, unsigned int seed, int extraPassages); // test helper
    static LoadResult loadFromString(const std::string& text);   // parse + validate; used by loadFromFile and tests
    static LoadResult loadFromFile(const std::string& path);

private:
    // hand-written validation helpers — no strtol/atoi/sscanf, no <cctype>:
    static bool parseNonNegativeInt(const std::string& token, int& outValue); // char-by-char digit loop
    static bool isHexCellChar(char c);        // '0'..'9', 'A'..'F', 'a'..'f'
    static bool isSpaceChar(char c);          // ' ', '\t', '\r', '\n'
};
```

Pure `std::string`/`fstream` parsing with hand-written helpers — see §6 for the format. **Fail-atomic:** everything parses into a temporary `LoadResult`; the caller only swaps it into live state when `ok`, so a failed load never corrupts the current maze.

### GUI classes (`src/gui/`)

```cpp
class MazeCanvas : public QWidget {
    Q_OBJECT
public:
    void setMaze(const MazeMatrix* maze, const IntLinkedList* bfsVisited, int visitedCount,
                 const IntLinkedList* route, int routeCount, int cursorRow, int cursorCol);
    // pointer set is NON-owning (MainWindow owns all core objects as members);
    // cursorRow==-1 hides the generation cursor; counts control how much is highlighted
protected:
    void paintEvent(QPaintEvent* event) override;
};

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void onGenerateClicked();
    void onSolveClicked();
    void onSaveClicked();
    void onLoadClicked();
    void onSearchLibraryClicked();
    void onAnimationFrame();          // single QTimer tick

private:
    // ---- core objects (owned as members; no raw new/delete in the GUI layer) ----
    MazeMatrix maze_;
    MazeGraph graph_;
    BfsSolver solver_;
    MazeLibrary library_;
    MazeMatrix animMatrix_;            // blank copy that replay ticks mutate

    // ---- replay state ----
    QTimer timer_;
    CarveStepList genSteps_;
    SolveStepList solveSteps_;
    IntLinkedList route_;
    int playhead_; int phase_;         // 0 = idle, 1 = generating, 2 = searching, 3 = drawing route
};
```

---

## 4. Algorithm notes (with viva one-liners)

**Recursive backtracking (generation).** Start at cell (0,0), mark visited, shuffle the four directions with `RandomLCG` (Fisher–Yates over a 4-element direction array), for each unvisited in-bounds neighbour: remove the wall between current and neighbour (clears both nibble sides), record a `CarveStep{Carve,...}`, recurse into the neighbour. When no unvisited neighbour remains, return — the call stack itself is the "step back". Then `openEntranceExit()` (north of (0,0), south of (rows−1,cols−1)). Result is a perfect maze: every cell reachable, exactly one path between any two cells. Depth ≤ 50×50 = 2500 ⇒ safe on a 1 MB stack at the enforced cap.
*Viva line:* "Depth-first carving — visit a random unvisited neighbour, knock down the wall between us, recurse; the call stack backtracks for me."

**Matrix → graph.** Vertex id = `row * cols + col` for *every* cell (all cells are rooms). Edge between adjacent cells iff the shared wall bit is cleared. Build: `adjacency_ = new IntLinkedList[rows_*cols_]`, one pass checking only each cell's E and S neighbours so each interior passage is added to both adjacency lists exactly once. The entrance/exit outside openings produce no edges because outside bounds are never vertices.
*Viva line:* "One vertex per cell; scan walls once, and every open passage becomes an edge in two adjacency lists."

**BFS with parent links.** `new` two scratch arrays of size V: `seen[V]` (0/1) and `parent[V]` (`parent[source] = -1`). Seed an `IntLinkedQueue` with the source; loop: dequeue `v`, record `SolveStep{Dequeued, v}`; if `v == target` → success, stop. Otherwise for each neighbour `w` in `neighbors(v)`: record `SolveStep{EdgeChecked, v, w, discovered}`; if `!seen[w]` → `seen[w]=1`, `parent[w]=v`, enqueue. If the queue drains without reaching the target → **no route**: return `false`, leave `outRoute` empty (GUI prints a status message; tests assert `false` + empty route).
*Viva line:* "FIFO order means the first time a cell is reached, it's via a shortest route; `parent[]` remembers who discovered each cell."

**Route reconstruction.** Walk `parent` from `target` back to `source`, `pushFront` each vertex id into `outRoute` — O(1) inserts give the list in entrance→exit order directly, no separate reversal pass. (Alternative: pushBack then reverse; rejected because `pushFront` avoids it.)
*Viva line:* "Walk the discovery chain backwards and pushFront each step."

**Extra openings.** After `generate()`: pick a random cell + random side via the continuing RNG stream; if that side's wall exists *and* the neighbour is in bounds, `removeWall` it and record `CarveStep{ExtraOpening, ...}`; repeat `count` times. Adds at most `count` new interior edges so BFS then has several candidate routes and is actually required to pick the shortest one. Deterministic for the same (seed, count) pair.

**BST insert/search/delete by seed.** Standard recursion on `Node*`. Search: compare, descend left (<) or right (>) until `nullptr` or match — O(h). Insert: descend to a `nullptr` and attach; if the seed matches an existing node, copy over the record (replace policy). Delete: leaf → free; one child → splice; two children → copy the in-order successor's record into the node, then `remove` the successor from the right subtree. `inOrder` appends records into `MazeRecordList` (sorted by seed) so the GUI can list the library.
*Viva line:* "Sorted insertion gives O(log n) search by seed; duplicate key = update in place."

---

## 5. Animation design (record–replay)

Core never blocks and never talks to Qt. Each long operation runs eagerly in the button handler (worst case 50×50: generation ~2500 carves, BFS ~2500 dequeues + ≤10k edge checks — sub-millisecond work) and records steps. The GUI then re-plays the recording:

**Generation replay.** `Generate` button → build `MazeGenerator`, `generate()`, optional `openExtraPassages()` → keep `genSteps_`. Also create `animMatrix_` = a fresh *all-walls* matrix of the same size (final `maze_` is untouched during animation). `QTimer` at 20 ms: each tick applies the next `CarveStep` (or a run of them — see below) into `animMatrix_` via `removeWall`, moves the paint cursor to that cell, `canvas->update()`. When the playhead reaches the end: copy the real `maze_` into `animMatrix_` (one assignment), stop the timer, build the graph, enable `Solve`. The canvas needs **no knowledge of generation** — it just paints a matrix, and the matrix visibly grows.

**BFS replay, two phases on the same timer.**
*Phase 1 — search:* tick = next `SolveStep`; the canvas highlights `visitedCount` cells (all vertices dequeued so far, previously discovered in BFS order) in green and the last edge checked in yellow. The GUI holds a cursor over `solveSteps_` and simply advances an int; the canvas walks the list from the head up to that count (O(n) per paint, n ≤ 2500 → at most ~6 M pointer hops over the whole animation — negligible).
*Phase 2 — route:* once steps are exhausted and the solver returned `true`, switch to drawing the `route_` list cell by cell from entrance to exit in red. On `false` (no route): stop immediately, status line shows "No route from entrance to exit".

**Constant watchable duration.** Unscaled 1-step-per-tick at 20 ms makes a 50×50 solve take ~70 s — too slow; a 15×15 generation would take <1 s — too fast. Fix: per tick apply `max(1, totalSteps / 200)` steps → every animation lasts ≈ 4 s regardless of maze size. (Alternative: fixed 1 step/tick plus a speed slider — rejected as extra UI; the scaling rule is one line of code.)

All GUI code follows the "no work in `paintEvent`, no `sleep`/`processEvents` anywhere" rule: button handlers run core code once and return to the event loop; the only thing `QTimer` does is advance a playhead and request repaints.

---

## 6. File formats

**Save format — strict, ordered, line-based; every line validated.**

```
MAZE_V1
rows 4
cols 5
seed 12345
extra 2
7 7 B 1 5
7 C 9 F D
5 1 B 6 4
3 C 7 A 2
```

- Line 1: exact literal `MAZE_V1` (magic).
- Line 2/3: `rows <int>` / `cols <int>` — each integer parsed by the **hand-written digit loop** `parseNonNegativeInt` (no `strtol`/`atoi`/`sscanf`), in range **1..50**; anything else = *"invalid size"*.
- Line 4/5: `seed <int>` and `extra <int>` — non-negative integers, same parser.
- Next `rows` lines: exactly `cols` whitespace-separated tokens (whitespace via the hand-written `isSpaceChar`), each **one** hex digit `0-9A-Fa-f` checked by `isHexCellChar` = the cell's wall nibble (N=1 E=2 S=4 W=8, bit set = wall). Entrance/exit outside walls appear here as cleared bits.
- No trailing content after the grid (whitespace-only lines tolerated). Any deviation: *"malformed grid at line N"* with the line number in `error`.
- Cell values are arbitrary from the parser's view — the loader does not re-solve connectivity, so an all-walls "maze" loads fine and BFS may report no route afterward. That is by design: loading = trusting the file's explicit grid.
- **Round-trip:** `saveToString` == `maze.toWallString()` split into rows lines; `loadFromString` rebuilds via `MazeMatrix::fromWallString` after full validation (length `rows*cols`, every char hex).
- **Fail-atomic:** the parse result is a `LoadResult` temporary; the GUI swaps it in *only* when `ok`, so an aborted load leaves the current maze untouched.
- Examples of **malformed** files the tests feed in: `MAZE_V2` magic, `rows 0`, `rows 51`, a row with too many/few cells, a token like `G`, a truncated grid, trailing junk — each must yield `ok==false` with a specific message.

The `extra` count is informational (the grid is the single source of truth on load); it's stored so the library record and the save file agree when a maze is re-saved.

---

## 7. Phased implementation roadmap

Order follows the one originally agreed, with **one exception**: Save/Load moved up from last position to directly before the GUI — it needs only the matrix (no queue/graph), it is core-only and small, and doing it first means the GUI phase ships with *all* its buttons working in one pass instead of two of them dead until phase 9. Everything else keeps the given order because each phase consumes exactly the previous one.

No-API audit rule applies from the first commit: before every code commit, grep `src/` and `tests/` for `std::vector|std::list|std::stack|std::queue|std::map|std::set|<random>|<chrono>|<cctype>|strtol|atoi|sscanf|runtime_error|QVector|QList|QQueue|QMap` — zero hits required.

| # | Milestone — goal | Files touched | Definition of done | Console tests proving it |
|---|---|---|---|---|
| **0** | Scaffold: CMake targets all build empty, kit configured | `CMakeLists.txt`, `CMakePresets.json`, `.gitignore`, empty stubs | `cmake --preset windows-qt && cmake --build build` succeeds; `maze_tests` runs, prints "0 tests"; `maze_gui` opens an empty window | — (build check, manual) |
| **1** | Matrix + ASCII print | `maze_matrix.h/.cpp`, `test_matrix.cpp` | Construct/copy/wall ops work; `reset()` validates 1..50 and returns false (leaving state untouched) for invalid; `toAscii()` matches a hand-written 3×3 golden | T01–T05 |
| **2** | `IntLinkedList` + `IntLinkedQueue` | `int_linked_list.h/.cpp`, `int_linked_queue.h/.cpp`, `test_lists.cpp` | pushBack/pushFront/popFront/clear correct, deep copies independent, queue FIFO order proven, drains safely | T06–T09 |
| **3** | Generation with seed (+ entrance/exit) | `rng.h/.cpp`, `maze_generator.h/.cpp`, `steps.h`, `test_generator.cpp` | Same seed ⇒ identical maze twice; different seed ⇒ (almost surely) different; every cell visited; interior open-edge count = rows*cols−1 (perfect maze); entrance+exit outside walls open; works for 1×1, 1×N, N×1, 50×50 | T10–T15 |
| **4** | Graph | `maze_graph.h/.cpp`, `test_graph_bfs.cpp` | Vertex count = rows*cols; adjacency symmetric; sum of degrees = 2·(interior edges); rebuild from a changed matrix is correct | T16–T18 |
| **5** | BFS + route | `bfs_solver.h/.cpp`, `test_graph_bfs.cpp` | Route on a hand-built tiny maze matches the known shortest; route verified end-to-end (adjacent/open-wall/no repeats/correct endpoints); queue-drain no-route case returns `false` with empty route; work bounded structurally (trace length ≤ 4·V) | T19–T23 |
| **6** | Extra openings | `maze_generator.cpp` (function only), `test_extra.cpp` | Opening k passages adds exactly min(k, available) interior edges; BFS route length after ≤ before; same (seed,count) ⇒ same result; extras run after generation and never disconnect the maze | T24–T26 |
| **7** | Save / load | `maze_io.h/.cpp`, `test_io.cpp` | Round-trip via string and file is bit-identical; every malformed variant is rejected with a specific error including a line number; failed load leaves live state untouched | T27–T31 |
| **8** | Qt GUI | `src/gui/*` | Window with rows/cols/seed/extra spin boxes (clamped 1..50 / 0..2^31−1) and Generate/Solve/Save/Load buttons; generation animates via `QTimer` and visibly carves; BFS replays then route draws; "no route" shows a status message; save/load via `QFileDialog` round-trip with a saved file; window never freezes | manual checklist M1–M6 (§8) |
| **9** | BST library | `maze_library.h/.cpp`, `test_library.cpp` + GUI wiring | Insert/find by seed; duplicate seed replaces; delete covers leaf/one-child/two-children; `inOrder` returns seeds ascending; GUI "Search" inserts the current maze and loads a match's maze into view | T32–T36 + M7 |

Live regression rule: `maze_tests` (all suites) must stay green at the end of **every** phase before the commit is made; after phase 8, also `--demo` the ASCII output for a visual sanity check without launching the GUI.

---

## 8. Test plan

Framework: `tests/test_util.h` provides `CHECK(cond)`, `CHECK_EQ(a,b)`, a per-suite counter, and a final summary; `test_main.cpp` calls all suites and returns non-zero on any failure. Note: there are deliberately **no timing assertions** (no `<chrono>`) — performance adequacy is a consequence of the data structures (BFS is O(V+E), V ≤ 2500) plus the manual responsiveness checks M3/M6; the structural work-bound in T15/T22 stands in for it.

| ID | Area | Input / action | Expected result |
|----|------|----------------|-----------------|
| T01 | Matrix | `MazeMatrix m; m.reset(1,1)` | 1×1, all 4 walls set, `toAscii()` matches the 1×1 golden string |
| T02 | Matrix | on a live 5×5 maze call `m.reset(0,5)`, `m.reset(51,51)`, `m.reset(-3,4)` | each returns **false**; matrix left byte-for-byte unchanged (invalid size handled without exceptions) |
| T03 | Matrix | `removeWall(0,0,WALL_E)` on empty 2×2 | clears E bit of (0,0) **and** W bit of (0,1) |
| T04 | Matrix | deep-copy a matrix, mutate the copy | original unchanged (rule-of-three test) |
| T05 | Matrix | `toWallString()`/`fromWallString()` round-trip | identical strings; `MazeIO::loadFromString` (the validation point) rejects bad hex or wrong length |
| T06 | Lists | pushBack 1,2,3 then popFront ×3 | yields 1,2,3 in order; 4th pop returns false |
| T07 | Lists | pushFront use (BFS route building) | head-first access, list preserved |
| T08 | Lists | copy list, append to copy | original intact; sizes differ correctly |
| T09 | Queue | enqueue a,b,c; dequeue; enqueue d | dequeues a,b,c,d — true FIFO under interleaving |
| T10 | Generation | `generate()` twice with seed 42 into two matrices | byte-identical wall strings (same seed ⇒ same maze) |
| T11 | Generation | seeds 1 vs 2 on 15×15 | wall strings differ (with near-certainty) |
| T12 | Generation | any seed, n×m | every cell visited; interior open-edge count == n*m−1 (perfect maze) |
| T13 | Generation | 1×1 | generates without touching walls; entrance == exit cell; BFS source == target |
| T14 | Generation | 1×N and N×1 | single corridor; BFS route length == N cells |
| T15 | Generation | 50×50 | completes; steps list size == 2499 carves; all cells visited |
| T16 | Graph | build on generated 5×5 | vertexCount == 25; every adjacency mirrored (edge appears in both lists) |
| T17 | Graph | sum of adjacency sizes | == 2 × interior edge count from matrix scan |
| T18 | Graph | 32×32 rebuild after `generate()` again | old lists freed, new counts correct (no stale data) |
| T19 | BFS | hand-built 3×3 with a known unique route | route vertex sequence equals the expected one; length == expected |
| T20 | BFS | route validity checker | consecutive vertices adjacent with the shared wall open; no repeats; starts at entrance, ends at exit |
| T21 | BFS | 2×2, all interior walls closed, entrance/exit outside walls open | `solve()` returns **false**, route empty, steps recorded to the drain point |
| T22 | BFS | 50×50 solved after phase 6 | trace length ≤ 4·V + route (structural work bound); route valid |
| T23 | BFS | 1×1 maze | source == target ⇒ immediately true, route == {vertex 0}, length 1 |
| T24 | Extra | open 5 passages on a fresh perfect maze | interior edge count increases by exactly 5; maze still fully connected (BFS finds a route) |
| T25 | Extra | same seed + count replayed | identical wall strings; a different count yields a different (still valid) maze |
| T26 | Extra | BFS before vs after extras | post-route length ≤ pre-route length (new edges can only help) |
| T27 | IO | save → load round-trip (string and file) | maze, seed, extra identical; `ok == true` |
| T28 | IO | wrong magic / `rows 0` / `rows 51` / negative | `ok == false`, error mentions "invalid size" or magic, nothing loaded |
| T29 | IO | row with wrong cell count, token `G`, truncated grid, trailing junk | `ok == false`, error includes the offending line number |
| T30 | IO | valid file using lowercase hex | accepted (a–f normalised) |
| T31 | IO | failed `loadFromFile` while a maze is live | current maze and library untouched (fail-atomic check) |
| T32 | BST | insert 3 records, `findBySeed` each | all found with identical records; missing seed → `false` |
| T33 | BST | insert record with duplicate seed | size unchanged, stored record replaced |
| T34 | BST | remove leaf / one-child / two-children nodes | tree stays searchable; `inOrder` still sorted ascending by seed |
| T35 | BST | `inOrder` on ≥5 mixed seeds | returns every record exactly once in ascending seed order |
| T36 | BST | empty tree find/remove | `false`, no crash |
| M1 | GUI manual | rows/cols/seed spins out of 1..50 / 0..2^31−1 | clamped by spin box; invalid size impossible to request |
| M2 | GUI manual | Generate twice, same seed | identical drawings |
| M3 | GUI manual | Generate 50×50 then Solve | animation finishes (~4s), window interactive during it, route shown |
| M4 | GUI manual | Solve a maze with no route (e.g. load all-walls file) | status bar shows "No route from entrance to exit", no crash |
| M5 | GUI manual | Save → Load (including a deliberately corrupted file) | round-trip works; corrupted file shows an error and does not replace the maze |
| M6 | GUI manual | max-size maze generation during resize/other clicks | UI stays responsive; no freeze |
| M7 | GUI manual | library search by seed after inserting several mazes | found maze loads into view; unknown seed prints "not found" |

---

## 9. Risks and mitigations

| Risk | Why it matters | Mitigation |
|------|----------------|------------|
| **Recursion depth** | 50×50 ⇒ ≤2500 frames; a deep call stack in some debug builds could overflow 1 MB | The cap is *enforced in `MazeMatrix::reset` (the only place sizes are created)*, so no caller can exceed it; measured cost ≈0.3 MB worst case on MSVC. Escalation path: convert `carve` to an explicit-stack loop using a custom stack linked list — kept recursive here because the course requires self-tracking recursion and the cap makes it provably safe. Test T15 (50×50) runs in every build as the smoke check. |
| **Scope/API discipline slipping** | An accidental `std::vector` or `strtol` would break the course rule and be hard to explain in the viva | The grep audit from §7 runs before every code commit (zero hits in `src/` and `tests/`); the allowed list (language, `std::string`, file streams, Qt Widgets for the four GUI jobs) is documented in the assumptions; hand-written replacements (integer parser, LCG, return codes) already exist in the design so there is no temptation to reach for library helpers. |
| **Memory leaks / double frees** | Manual `new/delete` in 5 classes | Uniform rules: (1) containers own their nodes, nodes never escape; (2) rule-of-three with deep copy on every class holding an owning pointer (matrix, graph, queue, list, BST); (3) copy-and-swap assignment; (4) `nullptr` before every `delete`, and `delete[]` before re-`new[]` on rebuild; (5) BFS scratch arrays freed at every return of `solve()`. Verification (optional, tooling only — no project code involved): MSVC `/fsanitize=address` on the test target; omit entirely when being checked for the viva. |
| **Missing route / degenerate mazes** | BFS over a fully-walled or loaded maze may fail | Designed behavior: `solve()` returns `false` + empty route; GUI prints a status message; T21 and M4 cover it. Loader accepts the grid as-is and never tries to fix bad grids. |
| **GUI freezing** | Naive per-frame recomputation or `processEvents` hacks | All core work happens once per button click (worst case trivial); the timer only bumps a playhead index and repaints; nothing runs in `paintEvent`; no sleeps. Playhead scaling keeps any animation ≈4 s regardless of size. |
| **Non-deterministic "same maze"** | Same code must give the same maze across runs and machines | Hand-rolled `RandomLCG` with a fixed recurrence (§3); no use of `<random>` or hash/address-dependent values; reproducibility tests (T10/T25) run on every change. |
| **Windows / Qt kit setup** | Wrong compiler/Qt mismatch; "Qt6 not found" is the most common failure here | `CMakePresets.json` pins generator and `CMAKE_PREFIX_PATH` to the installed kit (e.g. `C:/Qt/6.x/msvc2019_64` with the VS 2022 generator); VS Code CMake Tools selects the matching kit; compiler architecture must match the Qt build (x64 vs x64). Phase 0 exists solely to burn this risk down before any logic is written. Sharing an exe: run `windeployqt` against the release binary. |
| **ASCII/console on Windows** | Console code pages mangle box-drawing characters | `toAscii()` uses only `+ - |` and spaces — no Unicode anywhere in console output. |

---

## 10. Git workflow

- Branch `main` stays compiling and green at all times; one commit per roadmap phase, made **only after** its tests (and all previous suites) pass and the no-API grep audit from §7 shows zero hits.
- Code + its tests + plan-doc updates land in the same commit as the phase, so every commit is independently understandable.
- Suggested commit messages (conventional, short scope):
  1. `docs: add docs/PLAN.md implementation plan`
  2. `build: scaffold cmake targets and windows-qt preset`
  3. `feat(core): maze matrix with walls, ascii and wall-string`
  4. `feat(core): linked list and queue`
  5. `feat(core): seeded recursive backtracking generator`
  6. `feat(core): matrix-to-graph adapter`
  7. `feat(core): bfs with parent links and route reconstruction`
  8. `feat(core): extra passages`
  9. `feat(core): text save/load with validation`
  10. `feat(gui): animated generation and bfs visualization`
  11. `feat(core): maze library bst with seed search`
  12. `test: edge case sweep + hardening pass`
  13. `docs: final README polish and viva notes`
- Tag `v1.0` after phase 9 (GUI + library complete, all tests green, README final).
- `.gitignore` at repo root:

```
# CMake/MSVC build output
build/
build-*/
out/
CMakeUserPresets.json
*.user
.vs/
# Editor workspace state
.vscode/
.cache/
# Qt Creator leftovers
*.autosave
CMakeLists.txt.user
```

(`.vscode/` ignored by default — if kit settings are worth sharing they already live in the committed `CMakePresets.json`.)

---

## First three tasks

1. **Copy this plan into the project + Phase 0 — scaffold the build.** Write this file unmodified to `Maze-Solver/docs/PLAN.md` (Plan mode cannot); commit it as `docs: add docs/PLAN.md implementation plan`. Then create the root `CMakeLists.txt` + `CMakePresets.json` + `.gitignore` and empty target folders exactly per §2; get `maze_tests` and a bare `maze_gui` window to configure and build with the installed Qt 6 kit in VS Code. Commit `build: scaffold ...`. This is every Windows/Qt risk dying at once.
2. **Phase 1 — `MazeMatrix`.** Implement `src/core/maze_matrix.h/.cpp` (Cell bitmask, rule-of-three with copy-and-swap, `isValidSize` + `reset()` returning false for invalid sizes, `removeWall` symmetry, `toAscii`, `toWallString`/`fromWallString`) plus `tests/test_matrix.cpp` with T01–T05. Green suite → commit.
3. **Phase 2 — `IntLinkedList` and `IntLinkedQueue`.** Standalone DSA classes in `src/core/int_linked_list.h/.cpp` and `src/core/int_linked_queue.h/.cpp` per §3 (rule-of-three, `tail_` for O(1) pushBack, safe `popFront`/`dequeue`, no template classes per course rules) with `tests/test_lists.cpp` covering T06–T09. Green suite → commit. This unblocks phase 3's step recording.
