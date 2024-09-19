#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver/solver.hpp>

using ::testing::UnorderedElementsAre;

using namespace minesweeper::solver;

class SolverSolveTest : public ::testing::Test {
protected:
    minesweeper::Minefield init_field = {
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    BasicSolver basic;
    AdvancedSolver advanced;
    ProbableSolver probable;
};

TEST_F(SolverSolveTest, BasicNoFlaggable) {
    auto state = SolverState(init_field);
    EXPECT_EQ(basic.flaggable(state).size(), 0);
}

TEST_F(SolverSolveTest, BasicNoSafe) {
    auto state = SolverState(init_field);
    EXPECT_EQ(basic.safe(state).size(), 0);
}

TEST_F(SolverSolveTest, BasicFlaggable) {
    minesweeper::Minefield basic_flag_field = {
        { Minesweeper::COVERED, 2,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::FLAG,    3 },
        { Minesweeper::COVERED, Minesweeper::COVERED, 2 },
        { Minesweeper::COVERED, 3,                    1 }
    };
    auto state = SolverState(basic_flag_field);
    auto flaggable = basic.flaggable(state);
    EXPECT_THAT(flaggable, UnorderedElementsAre(
        state.get_node(0, 2),
        state.get_node(2, 1),
        state.get_node(2, 0),
        state.get_node(3, 0)
    ));
}

TEST_F(SolverSolveTest, BasicSafe) {
    minesweeper::Minefield basic_safe_field = {
        { Minesweeper::FLAG,    2,                    Minesweeper::COVERED },
        { 1,                    Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::FLAG,    2,                    Minesweeper::FLAG }
    };
    auto state = SolverState(basic_safe_field);
    auto safe = basic.safe(state);
    for (auto node : safe) {
        printf("%p\n", node);
    }
    EXPECT_THAT(safe, UnorderedElementsAre(
        state.get_node(1, 1),
        state.get_node(2, 0),
        state.get_node(2, 1),
        state.get_node(2, 2)
    ));
}