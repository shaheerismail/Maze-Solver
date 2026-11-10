# How to run, play and test Maze Solver

## Run the game (double-click works)

```
build\src\gui\maze_gui.exe
```

The Qt/MinGW DLLs are already copied next to the exe. To rebuild and redeploy:

```
& D:\qt-installed\Tools\CMake_64\bin\cmake.exe --build --preset windows-qt
& D:\qt-installed\6.12.0\mingw_64\bin\windeployqt.exe --release --no-compiler-runtime .\build\src\gui\maze_gui.exe
copy D:\qt-installed\Tools\mingw1310_64\bin\libgcc_s_seh-1.dll,libstdc++-6.dll,libwinpthread-1.dll .\build\src\gui\
```

## Playing

1. Pick **Rows/Cols (1-50)**, a **seed**, and **extra openings**.
2. **Generate maze** — watch the recursive backtracker carve it (yellow ring = carving cursor).
3. **Solve (BFS)** — green cells = BFS dequeued (visited) cells, yellow flash = edge being checked,
   then the red trail = the shortest route from the green entrance (top-left) to the blue exit (bottom-right).
4. Extra openings make several routes exist, so BFS has real work to do — try seeds with 0 vs 8 openings.
5. Save/Load uses the text format `MAZE_V1`. Add to library stores the maze in the BST keyed by seed;
   set the lookup seed and press "Look up by seed" to restore it.

### Play manually (the game part)

1. **Generate maze** (or load one), then click **Play manually (arrows/WASD)**.
2. Click the maze area once, then walk the pawn with **arrow keys** or **WASD**.
   Dark lines are walls: trying to walk through one flashes the wall red and you stay put.
3. Blue breadcrumbs mark every cell you have walked through.
4. Reach the **blue notch at the bottom-right** to win: the BFS shortest route is drawn in red
   and the status bar compares your move count with the optimum.

### Mines, keys and stars

- Every generated/loaded maze is seeded with **pixel-art bombs** and **pixel-art keys**;
  the layout is deterministic per seed (same seed = same field, like the walls).
- Walk onto a key to collect it. Step onto a bomb and the run ends instantly
  with an explosion burst — the status bar shows your keys collected and star tier.
- Placement is verified with the project's own BFS: a bomb candidate is rejected
  if it would cut the exit OR any key off, so the game is always winnable.
- **Star system at the finish:** all keys -> 3 stars, half -> 2, a quarter -> 1,
  none -> 0. Plan the route: grab as many keys as you dare, then find the exit.
- Even **Solve (BFS)** respects the mines: its red route is the bomb-safe optimum.

## Console tests (226 checks)

```
build\tests\maze_tests.exe
```

Suites: matrix, lists, generator, graph, bfs, extra, io, library — exit code 0 when green.
Edge coverage: 1x1 maze, single row/column, invalid sizes, malformed/corrupt save files,
no-route maze, 50x50 max size, same-seed reproducibility, duplicate BST seed update.
