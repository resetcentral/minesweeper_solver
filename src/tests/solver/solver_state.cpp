#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver/solver.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::Pair;

using namespace minesweeper::solver;
using minesweeper::Minefield;

class SolverStateTest : public ::testing::Test {
protected:
    SolverState* state;
    minesweeper::Minefield field_init = {
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
    };

    minesweeper::Minefield field2 = {
        { Tile(0),       Tile(1),       Tile::Flag },
        { Tile(2),       Tile(3),       Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered },
        { Tile::Covered, Tile::Covered, Tile::Covered }
    };

    virtual void SetUp() {
        state = new SolverState(field_init);
    }

    virtual void TearDown() {
        delete state;
    }
};

TEST(SolverStateConstructorTest, ZeroWidth) {
    Minefield m {};
    EXPECT_THROW(SolverState state(m), std::invalid_argument);
}

TEST(SolverStateConstructorTest, ZeroHeight) {
    Minefield m {{}};
    EXPECT_THROW(SolverState state(m), std::invalid_argument);
}

TEST_F(SolverStateTest, Width) {
    EXPECT_EQ(state->width(), 4);
}

TEST_F(SolverStateTest, Height) {
    EXPECT_EQ(state->height(), 3);
}

TEST_F(SolverStateTest, Select) {
    auto node = state->get_node(1, 1);
    state->set_selected(node);
    EXPECT_EQ(&state->selected(), node);
}

TEST_F(SolverStateTest, CoveredInitial) {
    EXPECT_EQ(state->covered().size(), 12);
}

TEST_F(SolverStateTest, CoveredEdgeInitial) {
    EXPECT_EQ(state->covered_edge().size(), 0);
}

TEST_F(SolverStateTest, NumberEdgeInitial) {
    EXPECT_EQ(state->hint_edge().size(), 0);
}

TEST_F(SolverStateTest, UpdateSingle) {
    auto node = state->get_node(1, 0);

    EXPECT_EQ(state->get_node(0, 0)->value(), Tile::Covered);
    EXPECT_EQ(node->value(), Tile::Covered);
    EXPECT_EQ(state->get_node(0, 1)->value(), Tile::Covered);
    EXPECT_EQ(state->get_node(1, 1)->value(), Tile::Covered);

    state->update(node, field2);

    EXPECT_EQ(state->get_node(0, 0)->value(), Tile::Covered);
    EXPECT_EQ(node->value(), Tile(2));
    EXPECT_EQ(state->get_node(0, 1)->value(), Tile::Covered);
    EXPECT_EQ(state->get_node(1, 1)->value(), Tile::Covered);
}

TEST_F(SolverStateTest, UpdateZero) {
    auto node1 = state->get_node(0, 0);
    auto node2 = state->get_node(1, 0);    
    node2->set_value(Tile(2));

    EXPECT_EQ(node1->value(), Tile::Covered);
    EXPECT_EQ(node2->value(), Tile(2));
    EXPECT_EQ(state->get_node(0, 1)->value(), Tile::Covered);
    EXPECT_EQ(state->get_node(1, 1)->value(), Tile::Covered);

    state->update(node1, field2);
    EXPECT_EQ(node1->value(), 0);
    EXPECT_EQ(node2->value(), 2);
    EXPECT_EQ(state->get_node(0, 1)->value(), 1);
    EXPECT_EQ(state->get_node(1, 1)->value(), 3);
}

TEST_F(SolverStateTest, UpdateFlag) {
    auto node = state->get_node(0, 0);
    auto flag = state->get_node(0, 2);
    state->update(node, field2);

    EXPECT_EQ(flag->value(), Tile::Covered);

    state->update(flag, field2);

    EXPECT_EQ(flag->value(), Tile::Flag);
    EXPECT_EQ(state->get_node(0, 1)->adjacent_mines_left(), 0);
    EXPECT_EQ(state->get_node(1, 1)->adjacent_mines_left(), 2);
}

TEST_F(SolverStateTest, Covered) {
    state->update(state->get_node(0, 0), field2);
    EXPECT_THAT(state->covered(), UnorderedElementsAre(
        state->get_node(0, 2),
        state->get_node(1, 2),
        state->get_node(2, 0), state->get_node(2, 1), state->get_node(2, 2),
        state->get_node(3, 0), state->get_node(3, 1), state->get_node(3, 2)
    ));
}

TEST_F(SolverStateTest, CoveredEdge) {
    state->update(state->get_node(0, 0), field2);
    EXPECT_THAT(state->covered_edge(), UnorderedElementsAre(
        state->get_node(0, 2),
        state->get_node(1, 2),
        state->get_node(2, 0), state->get_node(2, 1), state->get_node(2, 2)
    ));
}

TEST_F(SolverStateTest, NumberEdge) {
    state->update(state->get_node(0, 0), field2);
    EXPECT_THAT(state->hint_edge(), UnorderedElementsAre(
        state->get_node(0, 1),
        state->get_node(1, 0), state->get_node(1, 1)
    ));
}

TEST_F(SolverStateTest, ConstructorCoords) {
    EXPECT_THAT(state->get_node(0, 0)->coord(), Pair(0, 0));
    EXPECT_THAT(state->get_node(2, 2)->coord(), Pair(2, 2));
    EXPECT_THAT(state->get_node(3, 2)->coord(), Pair(3, 2));
}

TEST_F(SolverStateTest, ConstructorInitialValues) {
    EXPECT_EQ(state->get_node(0, 0)->value(), Tile::Covered);
    EXPECT_EQ(state->get_node(2, 2)->value(), Tile::Covered);
    EXPECT_EQ(state->get_node(3, 2)->value(), Tile::Covered);
}

TEST_F(SolverStateTest, ConstructorAdjacentCount) {
    EXPECT_EQ(state->get_node(0, 0)->adjacent().size(), 3);
    EXPECT_EQ(state->get_node(1, 0)->adjacent().size(), 5);
    EXPECT_EQ(state->get_node(2, 0)->adjacent().size(), 5);
    EXPECT_EQ(state->get_node(3, 0)->adjacent().size(), 3);
    EXPECT_EQ(state->get_node(0, 1)->adjacent().size(), 5);
    EXPECT_EQ(state->get_node(1, 1)->adjacent().size(), 8);
    EXPECT_EQ(state->get_node(2, 1)->adjacent().size(), 8);
    EXPECT_EQ(state->get_node(3, 1)->adjacent().size(), 5);
}

TEST_F(SolverStateTest, ConstructorAdjacent) {
    EXPECT_THAT(state->get_node(1, 1)->adjacent(), UnorderedElementsAre(
        state->get_node(0, 0), state->get_node(0, 1), state->get_node(0, 2), 
        state->get_node(1, 0),                        state->get_node(1, 2), 
        state->get_node(2, 0), state->get_node(2, 1), state->get_node(2, 2)
    ));

    EXPECT_THAT(state->get_node(3, 0)->adjacent(), UnorderedElementsAre(
        state->get_node(2, 0), state->get_node(2, 1),
                               state->get_node(3, 1)
    ));
}

TEST_F(SolverStateTest, ConstructorPartialMinefield) {
    auto partialState = SolverState(field2);
    EXPECT_EQ(partialState.get_node(0, 0)->value(), 0);
    EXPECT_EQ(partialState.get_node(0, 1)->value(), 1);
    EXPECT_EQ(partialState.get_node(0, 2)->value(), Tile::Flag);
    EXPECT_EQ(partialState.get_node(1, 0)->value(), 2);
    EXPECT_EQ(partialState.get_node(1, 1)->value(), 3);
    EXPECT_EQ(partialState.get_node(1, 2)->value(), Tile::Covered);
    EXPECT_EQ(partialState.get_node(2, 0)->value(), Tile::Covered);
    EXPECT_EQ(partialState.get_node(2, 1)->value(), Tile::Covered);
}

TEST_F(SolverStateTest, ConstructorPartialAdjacentMines) {
    auto partialState = SolverState(field2);
    EXPECT_EQ(partialState.get_node(0, 0)->adjacent_mines_left(), 0);
    EXPECT_EQ(partialState.get_node(0, 1)->adjacent_mines_left(), 0);
    EXPECT_EQ(partialState.get_node(0, 2)->adjacent_mines_left(), 0);
    EXPECT_EQ(partialState.get_node(1, 0)->adjacent_mines_left(), 2);
    EXPECT_EQ(partialState.get_node(1, 1)->adjacent_mines_left(), 2);
    EXPECT_EQ(partialState.get_node(1, 2)->adjacent_mines_left(), 0);
    EXPECT_EQ(partialState.get_node(2, 0)->adjacent_mines_left(), 0);
    EXPECT_EQ(partialState.get_node(2, 1)->adjacent_mines_left(), 0);
}