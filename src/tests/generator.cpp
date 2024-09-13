#include <gtest/gtest.h>
#include <minesweeper.hpp>

using namespace minesweeper;

TEST(MinefieldGeneratorTest, ThreeByThree) {
  MinefieldGenerator gen { 5001 };
  Minefield field = gen.generate(3, 3, 3);
  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      printf("%d ", field[i][j]);
    }
    printf("\n");
  }

  unsigned short expected[3][3] = {
    { 0, 1, 1 },
    { 2, 3, 9 },
    { 9, 9, 2 }
  };
  
  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      EXPECT_EQ(field[i][j], expected[i][j]);
    }
  }
}

TEST(MinefieldGeneratorTest, CornerMines1) {
  MinefieldGenerator gen { 9 };
  Minefield field = gen.generate(3, 3, 4);

  unsigned short expected[3][3] = {
    { 9, 2, 1 },
    { 3, 9, 2 },
    { 9, 3, 9 }
  };
  
  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      EXPECT_EQ(field[i][j], expected[i][j]);
    }
  }
}

TEST(MinefieldGeneratorTest, CornerMines2) {
  MinefieldGenerator gen { 16 };
  Minefield field = gen.generate(3, 3, 4);

  unsigned short expected[3][3] = {
    { 2, 9, 9 },
    { 2, 9, 9 },
    { 1, 2, 2 }
  };
  
  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      EXPECT_EQ(field[i][j], expected[i][j]);
    }
  }
}