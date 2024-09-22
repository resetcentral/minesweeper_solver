#include <solver/solver.hpp>

int main() {
    // Get height, width, num mines (or give options: Beginner, intermediate, expert)

    minesweeper::Minesweeper game(30, 16, 99);
    minesweeper::solver::StateLogger logger;
    minesweeper::solver::MinesweeperSolver solver(game, logger);
    solver.solve();
}