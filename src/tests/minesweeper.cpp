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

TEST(MinesweeperGetTile, StartCovered) {
    auto width = 4;
    auto height = 7;
    Minesweeper game ( width, height, 9 );

    for (auto x = 0; x < width; x++) {
        for (auto y = 0; y < height; y++) {
            EXPECT_EQ(game.get_tile(x, y), Minesweeper::COVERED);
        }
    }
}

TEST(MinesweeperGetTile, OutOfBoundsX) {
    Minesweeper game ( 9, 12, 14 );
    EXPECT_THROW(game.get_tile(9, 5), std::out_of_range);
}

TEST(MinesweeperGetTile, OutOfBoundsY) {
    Minesweeper game ( 9, 12, 14 );
    EXPECT_THROW(game.get_tile(3, 12), std::out_of_range);
}

TEST(MinesweeperToggleFlag, OutOfBoundsX) {
    Minesweeper game ( 9, 12, 14 );
    EXPECT_THROW(game.toggle_flag(9, 5), std::out_of_range);
}

TEST(MinesweeperToggleFlag, OutOfBoundsY) {
    Minesweeper game ( 9, 12, 14 );
    EXPECT_THROW(game.toggle_flag(3, 12), std::out_of_range);
}

TEST(MinesweeperToggleFlag, OnAndOff) {
    Minesweeper game ( 9, 12, 14 );

    EXPECT_EQ(game.get_tile(0, 0), Minesweeper::COVERED);
    EXPECT_EQ(game.flags_placed_count(), 0);
    game.toggle_flag(0, 0);
    EXPECT_EQ(game.get_tile(0, 0), Minesweeper::FLAG);
    EXPECT_EQ(game.flags_placed_count(), 1);

    game.toggle_flag(0, 0);
    EXPECT_EQ(game.get_tile(0, 0), Minesweeper::COVERED);
    EXPECT_EQ(game.flags_placed_count(), 0);
}

TEST(MinesweeperToggleFlag, MultiFlag) {
    Minesweeper game ( 9, 12, 14 );

    EXPECT_EQ(game.get_tile(1, 5), Minesweeper::COVERED);
    EXPECT_EQ(game.get_tile(3, 7), Minesweeper::COVERED);
    EXPECT_EQ(game.get_tile(8, 2), Minesweeper::COVERED);
    EXPECT_EQ(game.flags_placed_count(), 0);

    game.toggle_flag(1, 5);
    EXPECT_EQ(game.get_tile(1, 5), Minesweeper::FLAG);
    EXPECT_EQ(game.flags_placed_count(), 1);

    game.toggle_flag(3, 7);
    EXPECT_EQ(game.get_tile(3, 7), Minesweeper::FLAG);
    EXPECT_EQ(game.flags_placed_count(), 2);    

    game.toggle_flag(8, 2);
    EXPECT_EQ(game.get_tile(8, 2), Minesweeper::FLAG);
    EXPECT_EQ(game.flags_placed_count(), 3);

    game.toggle_flag(3, 7);
    EXPECT_EQ(game.get_tile(3, 7), Minesweeper::COVERED);
    EXPECT_EQ(game.flags_placed_count(), 2);  

    game.toggle_flag(1, 5);
    EXPECT_EQ(game.get_tile(1, 5), Minesweeper::COVERED);
    EXPECT_EQ(game.flags_placed_count(), 1);

    game.toggle_flag(8, 2);
    EXPECT_EQ(game.get_tile(8, 2), Minesweeper::COVERED);
    EXPECT_EQ(game.flags_placed_count(), 0);
}

// uncover tile tests
// - 5
// - 0 in middle
// - 0 in upper left corner
// - 0 in lower right corner
// - mine
// throw on out of bounds indices
// check that tile state is changed