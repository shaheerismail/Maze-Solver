


#include "maze_generator.h"


MazeGenerator::MazeGenerator(MazeMatrix& maze, unsigned int seed)
    : maze_(maze), rng_(seed)
{
}


void MazeGenerator::generate()
{
    steps_.clear();
    if (!maze_.inBounds(0, 0))
        return;
    maze_.reset(maze_.rows(), maze_.cols());
    carve(0, 0);
    maze_.openOutside(0, 0, MazeMatrix::WALL_N);
    maze_.openOutside(maze_.rows() - 1, maze_.cols() - 1, MazeMatrix::WALL_S);
}


const CarveStepList& MazeGenerator::steps() const
{
    return steps_;
}



void MazeGenerator::carve(int row, int col)
{
    maze_.setVisited(row, col, true);
    int sides[4] = { MazeMatrix::WALL_N, MazeMatrix::WALL_E,
                     MazeMatrix::WALL_S, MazeMatrix::WALL_W };
    for (int i = 3; i > 0; --i) {
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
            CarveStep step;
            step.kind = CarveStep::Carve;
            step.row = row;
            step.col = col;
            step.side = sides[i];
            steps_.pushBack(step);
            maze_.removeWall(row, col, sides[i]);
            carve(nr, nc);
        }
    }
}





void MazeGenerator::openExtraPassages(int count)
{
    int available = 0;
    for (int r = 0; r < maze_.rows(); ++r) {
        for (int c = 0; c < maze_.cols(); ++c) {
            if (r > 0 && maze_.hasWall(r, c, MazeMatrix::WALL_N))
                ++available;
            if (c > 0 && maze_.hasWall(r, c, MazeMatrix::WALL_W))
                ++available;
        }
    }
    while (count > 0 && available > 0) {
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
        if (maze_.inBounds(nr, nc) && maze_.hasWall(row, col, side)) {
            CarveStep step;
            step.kind = CarveStep::ExtraOpening;
            step.row = row;
            step.col = col;
            step.side = side;
            steps_.pushBack(step);
            maze_.removeWall(row, col, side);
            --available;
            --count;
        }

    }
}
