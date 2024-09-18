#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver.hpp>

using ::testing::ElementsAre;
using namespace::minesweeper::solver;

TEST(Precomputed, Factorial) {
    EXPECT_THAT(factorial_list<8>(),
        ElementsAre(1, 1, 2, 6, 24, 120, 720, 5'040, 40'320));
}

TEST(Precomputed, Choose) {
    auto choose = choose_matrix<8,8>();
    EXPECT_THAT(choose,
        ElementsAre(
            ElementsAre(1, 0, 0, 0, 0, 0, 0, 0, 0),
            ElementsAre(1, 1, 0, 0, 0, 0, 0, 0, 0),
            ElementsAre(1, 2, 1, 0, 0, 0, 0, 0, 0),
            ElementsAre(1, 3, 3, 1, 0, 0, 0, 0, 0),
            ElementsAre(1, 4, 6, 4, 1, 0, 0, 0, 0),
            ElementsAre(1, 5, 10, 10, 5, 1, 0, 0, 0),
            ElementsAre(1, 6, 15, 20, 15, 6, 1, 0, 0),
            ElementsAre(1, 7, 21, 35, 35, 21, 7, 1, 0),
            ElementsAre(1, 8, 28, 56, 70, 56, 28, 8, 1)
        )
    );
}