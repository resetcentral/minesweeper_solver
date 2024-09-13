#include <gtest/gtest.h>
#include <minesweeper.hpp>

using namespace minesweeper;

TEST(MinesweeperConstructor, InvalidDimensionsWidth) {
    EXPECT_THROW(Minesweeper(0, 5, 2), std::domain_error);
}

TEST(MinesweeperConstructor, InvalidDimensionsHeight) {
    EXPECT_THROW(Minesweeper(3, 0, 2), std::domain_error);
}

TEST(MinesweeperConstructor, TooManyMinesEq) {
    EXPECT_THROW(Minesweeper(3, 3, 9), std::invalid_argument);
}

TEST(MinesweeperConstructor, TooManyMinesGt) {
    EXPECT_THROW(Minesweeper(5, 5, 26), std::invalid_argument);
}

class MinesweeperTest : public ::testing::Test {
    protected:
    Minesweeper* game;

    virtual void SetUp() {
        MinefieldGenerator gen { 100 };
        this->game = new Minesweeper (gen, 9, 12, 14 );
    }

    virtual void TearDown() {
        delete this->game;
    }
};

TEST_F(MinesweeperTest, GetTileStartCovered) {
    for (auto x = 0; x < 9; x++) {
        for (auto y = 0; y < 12; y++) {
            EXPECT_EQ(game->get_tile(x, y), Minesweeper::COVERED);
        }
    }
}

TEST_F(MinesweeperTest, GetTileOutOfBoundsX) {
    EXPECT_THROW(game->get_tile(9, 5), std::out_of_range);
}

TEST_F(MinesweeperTest, GetTileOutOfBoundsY) {
    EXPECT_THROW(game->get_tile(3, 12), std::out_of_range);
}

TEST_F(MinesweeperTest, ToggleFlagOutOfBoundsX) {
    EXPECT_THROW(game->toggle_flag(9, 5), std::out_of_range);
}

TEST_F(MinesweeperTest, ToggleFlagOutOfBoundsY) {
    EXPECT_THROW(game->toggle_flag(3, 12), std::out_of_range);
}

TEST_F(MinesweeperTest, ToggleFlagOnAndOff) {
    EXPECT_EQ(game->get_tile(0, 0), Minesweeper::COVERED);
    EXPECT_EQ(game->flags_placed_count(), 0);
    game->toggle_flag(0, 0);
    EXPECT_EQ(game->get_tile(0, 0), Minesweeper::FLAG);
    EXPECT_EQ(game->flags_placed_count(), 1);

    game->toggle_flag(0, 0);
    EXPECT_EQ(game->get_tile(0, 0), Minesweeper::COVERED);
    EXPECT_EQ(game->flags_placed_count(), 0);
}

TEST_F(MinesweeperTest, ToggleFlagMultiFlag) {
    EXPECT_EQ(game->get_tile(1, 5), Minesweeper::COVERED);
    EXPECT_EQ(game->get_tile(3, 7), Minesweeper::COVERED);
    EXPECT_EQ(game->get_tile(8, 2), Minesweeper::COVERED);
    EXPECT_EQ(game->flags_placed_count(), 0);

    game->toggle_flag(1, 5);
    EXPECT_EQ(game->get_tile(1, 5), Minesweeper::FLAG);
    EXPECT_EQ(game->flags_placed_count(), 1);

    game->toggle_flag(3, 7);
    EXPECT_EQ(game->get_tile(3, 7), Minesweeper::FLAG);
    EXPECT_EQ(game->flags_placed_count(), 2);    

    game->toggle_flag(8, 2);
    EXPECT_EQ(game->get_tile(8, 2), Minesweeper::FLAG);
    EXPECT_EQ(game->flags_placed_count(), 3);

    game->toggle_flag(3, 7);
    EXPECT_EQ(game->get_tile(3, 7), Minesweeper::COVERED);
    EXPECT_EQ(game->flags_placed_count(), 2);  

    game->toggle_flag(1, 5);
    EXPECT_EQ(game->get_tile(1, 5), Minesweeper::COVERED);
    EXPECT_EQ(game->flags_placed_count(), 1);

    game->toggle_flag(8, 2);
    EXPECT_EQ(game->get_tile(8, 2), Minesweeper::COVERED);
    EXPECT_EQ(game->flags_placed_count(), 0);
}

TEST_F(MinesweeperTest, UncoverTileOutOfBoundsX) {
    EXPECT_THROW(game->toggle_flag(9, 5), std::out_of_range);
}

TEST_F(MinesweeperTest, UncoverTileOutOfBoundsY) {
    EXPECT_THROW(game->toggle_flag(3, 12), std::out_of_range);
}

// uncover tile tests
// - 5
// - 0 in middle
// - 0 in upper left corner
// - 0 in lower right corner
// - mine
// throw on out of bounds indices
// check that tile state is changed