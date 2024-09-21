#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver/solver.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::AnyOf;
using ::testing::Pair;

using namespace minesweeper::solver;

class SubSolverTest : public ::testing::Test {
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

TEST_F(SubSolverTest, BasicNoFlaggable) {
    auto state = SolverState(init_field);
    EXPECT_EQ(basic.flaggable(state).size(), 0);
}

TEST_F(SubSolverTest, BasicNoSafe) {
    auto state = SolverState(init_field);
    EXPECT_EQ(basic.safe(state).size(), 0);
}

TEST_F(SubSolverTest, BasicFlaggable) {
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

TEST_F(SubSolverTest, BasicSafe) {
    minesweeper::Minefield basic_safe_field = {
        { Minesweeper::FLAG,    2,                    Minesweeper::COVERED },
        { 1,                    Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::FLAG,    2,                    Minesweeper::FLAG }
    };
    auto state = SolverState(basic_safe_field);
    auto safe = basic.safe(state);
    
    EXPECT_THAT(safe, UnorderedElementsAre(
        state.get_node(1, 1),
        state.get_node(2, 0),
        state.get_node(2, 1),
        state.get_node(2, 2)
    ));
}

TEST_F(SubSolverTest, AdvancedSolve1Safe) {
    minesweeper::Minefield advanced_field = {
        { 1,                    1,                    1 },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(advanced_field);
    auto decisions = advanced.solve(state, 5);

    EXPECT_THAT(decisions, UnorderedElementsAre(
        Pair(state.get_node(1, 2), false)
    ));
}

TEST_F(SubSolverTest, AdvancedSolve3Safe) {
    minesweeper::Minefield advanced_field = {
        { 1,                    Minesweeper::COVERED, Minesweeper::FLAG },
        { Minesweeper::COVERED, 3,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::FLAG },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(advanced_field);
    auto decisions = advanced.solve(state, 5);

    EXPECT_THAT(decisions, UnorderedElementsAre(
        Pair(state.get_node(1, 2), false),
        Pair(state.get_node(2, 0), false),
        Pair(state.get_node(2, 1), false)
    ));
}

TEST_F(SubSolverTest, AdvancedSolve2Flag) {
    minesweeper::Minefield advanced_field = {
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { 4,                    Minesweeper::COVERED, 2 },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(advanced_field);
    auto flaggable = advanced.flaggable(state);

    EXPECT_THAT(flaggable, UnorderedElementsAre(
        state.get_node(0, 0),
        state.get_node(2, 0)
    ));
}

TEST_F(SubSolverTest, AdvancedSolveExistingFlag) {
    minesweeper::Minefield advanced_field = {
        { Minesweeper::FLAG,    2,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { 4,                    Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(advanced_field);
    auto flaggable = advanced.flaggable(state);

    EXPECT_THAT(flaggable, UnorderedElementsAre(
        state.get_node(2, 1),
        state.get_node(3, 0),
        state.get_node(3, 1)
    ));
}

TEST_F(SubSolverTest, AdvancedSolve2Safe) {
    minesweeper::Minefield advanced_field = {
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { 3,                    Minesweeper::COVERED, 1 },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(advanced_field);
    auto safe = advanced.safe(state);

    EXPECT_THAT(safe, UnorderedElementsAre(
        state.get_node(0, 2),
        state.get_node(2, 2)
    ));


}

TEST_F(SubSolverTest, ProbableSolve1LowMines) {
    minesweeper::Minefield probable_field = {
        { Minesweeper::COVERED, 1,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(probable_field);
    auto safeish = probable.solve(state, 2);

    EXPECT_THAT(safeish, AnyOf(
        state.get_node(2, 0), state.get_node(2, 1), state.get_node(2, 2), 
        state.get_node(3, 0), state.get_node(3, 1), state.get_node(3, 2)
    ));
}

TEST_F(SubSolverTest, ProbableSolve1HighMines) {
    minesweeper::Minefield probable_field = {
        { Minesweeper::COVERED, 1,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(probable_field);
    auto safeish = probable.solve(state, 5);

    EXPECT_THAT(safeish, AnyOf(
        state.get_node(0, 0),                       state.get_node(0, 2), 
        state.get_node(1, 0), state.get_node(1, 1), state.get_node(1, 2)
    ));
}

TEST_F(SubSolverTest, ProbableSolve2) {
    minesweeper::Minefield probable_field = {
        { Minesweeper::COVERED, 1,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, 2,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, Minesweeper::COVERED }
    };
    auto state = SolverState(probable_field);

    EXPECT_EQ(advanced.solve(state, 3).size(), 0);

    auto safeish = probable.solve(state, 3);

    EXPECT_THAT(safeish, AnyOf(
        state.get_node(0, 0),                       state.get_node(0, 2), 
        state.get_node(1, 0), state.get_node(1, 1), state.get_node(1, 2)
    ));
}

TEST_F(SubSolverTest, ProbableSolveHard) {
    minesweeper::Minefield probable_hard_field = {
        { Minesweeper::COVERED, 2,                    Minesweeper::COVERED },
        { Minesweeper::COVERED, Minesweeper::COVERED, 1 },
        { Minesweeper::COVERED, 2,                    Minesweeper::COVERED}
    };
    auto state = SolverState(probable_hard_field);

    auto safeish = probable.solve(state, 3);

    EXPECT_THAT(safeish, AnyOf(
        state.get_node(0, 2),
        state.get_node(2, 2)
    ));
}