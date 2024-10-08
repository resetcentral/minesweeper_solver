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
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
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
        { Tile::Covered, Tile(2),       Tile::Covered },
        { Tile::Covered, Tile::Flag,    Tile(3) },
        { Tile::Covered, Tile::Covered, Tile(2) },
        { Tile::Covered, Tile(3),       Tile(1) }
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
        { Tile::Flag,    Tile(2),       Tile::Covered },
        { Tile(1),       Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Flag,    Tile(2),       Tile::Flag }
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
        { Tile(1),       Tile(1),       Tile(1) },
        { Tile::Covered, Tile::Covered, Tile::Covered }
    };
    auto state = SolverState(advanced_field);
    auto safe = advanced.safe(state);

    EXPECT_THAT(safe, UnorderedElementsAre(
        state.get_node(1, 2),
        state.get_node(1, 0)
    ));
}

TEST_F(SubSolverTest, AdvancedSolve3Safe) {
    minesweeper::Minefield advanced_field = {
        { Tile(1),       Tile::Covered, Tile::Flag },
        { Tile::Covered, Tile(3),       Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Flag }
    };
    auto state = SolverState(advanced_field);
    auto safe = advanced.safe(state);

    EXPECT_THAT(safe, UnorderedElementsAre(
        state.get_node(1, 2),
        state.get_node(2, 0),
        state.get_node(2, 1)
    ));
}

TEST_F(SubSolverTest, AdvancedSolve2Flag) {
    minesweeper::Minefield advanced_field = {
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile(4),       Tile::Covered, Tile(2) },
        { Tile::Covered, Tile::Covered, Tile::Covered }
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
        { Tile::Flag,    Tile(2),       Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile(4),       Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
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
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile(3),       Tile::Covered, Tile(1) },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
    };
    auto state = SolverState(advanced_field);
    auto safe = advanced.safe(state);

    EXPECT_THAT(safe, UnorderedElementsAre(
        state.get_node(0, 2),
        state.get_node(2, 2)
    ));


}

TEST_F(SubSolverTest, ProbableCalculate1LowMines) {
    minesweeper::Minefield probable_field = {
        { Tile::Covered, Tile(1),       Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
    };
    auto state = SolverState(probable_field);
    probable.calculate_probability(state, 3);

    Fraction one_fifth{1, 5};
    Fraction one_third{1, 3};
    EXPECT_EQ(state.get_node(0, 0)->mine_probability(), one_fifth);
    EXPECT_EQ(state.get_node(0, 2)->mine_probability(), one_fifth);
    EXPECT_EQ(state.get_node(1, 0)->mine_probability(), one_fifth);
    EXPECT_EQ(state.get_node(1, 1)->mine_probability(), one_fifth);
    EXPECT_EQ(state.get_node(1, 2)->mine_probability(), one_fifth);
    EXPECT_EQ(state.get_node(2, 1)->mine_probability(), one_third);
    EXPECT_EQ(state.get_node(3, 2)->mine_probability(), one_third);
}

TEST_F(SubSolverTest, ProbableCalculate) {
    minesweeper::Minefield probable_field = {
        { Tile::Covered, Tile(3),       Tile::Flag },
        { Tile::Covered, Tile::Covered, Tile(2) },
        { Tile::Flag,    Tile::Covered, Tile(1) }
    };
    auto state = SolverState(probable_field);

    probable.calculate_probability(state, 6); // assume this is a subsection of a larger board

    Fraction one_third {1, 3};
    Fraction two_thirds {2, 3};
    EXPECT_EQ(state.get_node(0, 0)->mine_probability(), two_thirds);
    EXPECT_EQ(state.get_node(1, 0)->mine_probability(), two_thirds);
    EXPECT_EQ(state.get_node(1, 1)->mine_probability(), two_thirds);
    EXPECT_EQ(state.get_node(2, 1)->mine_probability(), one_third);
}

TEST_F(SubSolverTest, ProbableCalculateHard) {
    minesweeper::Minefield probable_hard_field = {
        { Tile::Covered, Tile(2),                    Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile(1) },
        { Tile::Covered, Tile(2),                    Tile::Covered }
    };
    auto state = SolverState(probable_hard_field);

    probable.calculate_probability(state, 6); // assume this is a subsection of a larger board

    Fraction one_half {1, 2};
    Fraction one_quarter {1, 4};
    Fraction three_quarters {3, 4};
    EXPECT_EQ(state.get_node(0, 0)->mine_probability(), one_half);
    EXPECT_EQ(state.get_node(0, 2)->mine_probability(), one_quarter);
    EXPECT_EQ(state.get_node(1, 0)->mine_probability(), three_quarters);
    EXPECT_EQ(state.get_node(1, 1)->mine_probability(), one_half);
    EXPECT_EQ(state.get_node(2, 0)->mine_probability(), one_half);
    EXPECT_EQ(state.get_node(2, 2)->mine_probability(), one_quarter);
}

TEST_F(SubSolverTest, ProbableCalculateManyEquations) {
    minesweeper::Minefield probable_field = {
        { Tile::Flag, Tile(3),       Tile(2),       Tile::Flag },
        { Tile::Flag, Tile::Covered, Tile::Covered, Tile(2) },
        { Tile::Flag, Tile(4),       Tile::Covered, Tile::Covered },
        { Tile(1),    Tile(2),       Tile::Covered, Tile::Covered }
    };
    auto state = SolverState(probable_field);
    probable.calculate_probability(state, 5);

    Fraction one_third {1, 3};
    Fraction two_thirds {2, 3};
    EXPECT_EQ(state.get_node(1, 1)->mine_probability(), two_thirds);
    EXPECT_EQ(state.get_node(1, 2)->mine_probability(), one_third);
    EXPECT_EQ(state.get_node(2, 2)->mine_probability(), one_third);
    EXPECT_EQ(state.get_node(2, 3)->mine_probability(), one_third);
    EXPECT_EQ(state.get_node(3, 2)->mine_probability(), two_thirds);
}

TEST_F(SubSolverTest, ProbableSolve1HighMines) {
    minesweeper::Minefield probable_field = {
        { Tile::Covered, Tile(1),       Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
    };
    auto state = SolverState(probable_field);
    auto safeish = probable.solve(state, 5);

    EXPECT_THAT(safeish, AnyOf(
        state.get_node(0, 0),                       state.get_node(0, 2), 
        state.get_node(1, 0), state.get_node(1, 1), state.get_node(1, 2)
    ));
}

TEST_F(SubSolverTest, ProbableSolveHard) {
    minesweeper::Minefield probable_hard_field = {
        { Tile::Covered, Tile(2),       Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile(1) },
        { Tile::Covered, Tile(2),       Tile::Covered }
    };
    auto state = SolverState(probable_hard_field);

    auto safeish = probable.solve(state, 5); // assume this is a subsection of a larger board

    EXPECT_THAT(safeish, AnyOf(
        state.get_node(0, 2),
        state.get_node(2, 2)
    ));
}