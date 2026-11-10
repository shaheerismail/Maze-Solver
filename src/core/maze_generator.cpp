#include "maze_generator.h"

MazeGenerator::MazeGenerator(MazeMatrix& maze, unsigned int seed)
    : maze_(maze), rng_(seed)
{
}

void MazeGenerator::generate()
{
    steps_.clear();
    if (!maze_.inBounds(0, 0))
        return;  // empty matrix: nothing to carve
    maze_.reset(maze_.rows(), maze_.cols());  // close everything, unmark visited
    carve(0, 0);
    maze_.openOutside(0, 0, MazeMatrix::WALL_N);   // entrance: north of top-left
    maze_.openOutside(maze_.rows() - 1, maze_.cols() - 1, MazeMatrix::WALL_S); // exit
}

const LinkedList<GenStep>& MazeGenerator::steps() const
{
    return steps_;
}

// Depth-first carving: shuffle the four directions, knock down the wall to a
// random unvisited neighbour, recurse; the call stack itself backtracks.
void MazeGenerator::carve(int row, int col)
{
    maze_.setVisited(row, col, true);
    int sides[4] = { MazeMatrix::WALL_N, MazeMatrix::WALL_E,
                     MazeMatrix::WALL_S, MazeMatrix::WALL_W };
    for (int i = 3; i > 0; --i) {           // Fisher-Yates shuffle, seed-controlled
        int j = rng_.nextInt(0, i);
        int tmp = sides[i];
        sides[i] = sides[j];
        sides[j] = tmp;
    }
    for (int i = 0; i < 4; ++i) {
        int drow;
        int dcol;
        MazeMatrix::sideDelta(sides[i], drow, dcol);
        int nr = row + drow;
        int nc = col + dcol;
        if (maze_.inBounds(nr, nc) && !maze_.isVisited(nr, nc)) {
            GenStep step;
            step.kind = GenStep::Carve;
            step.row = row;
            step.col = col;
            step.side = sides[i];
            steps_.pushBack(step);
            maze_.removeWall(row, col, sides[i]);
            carve(nr, nc);
        }
    }
}

// Removes up to 'count' interior walls at random grid positions, continuing
// the SAME random stream as generate() so a (seed, count) pair is reproducible.
void MazeGenerator::openExtraPassages(int count)

{
    while (count > 0) {
        int row = rng_.nextInt(0, maze_.rows() - 1);
        int col = rng_.nextInt(0, maze_.cols() - 1);
        int sides[4] = { MazeMatrix::WALL_N, MazeMatrix::WALL_E,
                         MazeMatrix::WALL_S, MazeMatrix::WALL_W };
        int side = sides[rng_.nextInt(0, 3)];
        int drow;
        int dcol;
        MazeMatrix::sideDelta(side, drow, dcol);
        int nr = row + drow;
        int nc = col + dcol;
        if (maze_.inBounds(nr, nc) && maze_.hasWall(row, col, side)) {  // interior + closed
            GenStep step;
            step.kind = GenStep::Extra;
            step.row = row;
            step.col = col;
            step.side = side;
            steps_.pushBack(step);
            maze_.removeWall(row, col, side);
            --count;
        }
        // else: outside wall already open or 1x1 maze -> draw again
    }
}
