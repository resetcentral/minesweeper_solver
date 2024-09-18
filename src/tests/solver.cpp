#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver.hpp>

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;
using ::testing::Pair;
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

TEST(SolverNode, Coord) {
    auto node = Node(3, 6);

    std::pair<unsigned int, unsigned int> expected{ 3, 6 };
    EXPECT_EQ(node.coord(), expected);
}

TEST(SolverNode, AddAdjacent) {
    auto node1 = Node(3, 6);
    auto node2 = Node(4, 6);
    
    node1.add_adjacent(&node2);
    EXPECT_THAT(node1.adjacent(), ElementsAre(&node2));
    EXPECT_THAT(node2.adjacent(), ElementsAre(&node1));
}

class NodeTest : public ::testing::Test {
    protected:
    std::vector<std::vector<Node>> nodes;

    virtual void SetUp() {
        for (auto x = 0; x < 3; x++) {
            nodes.push_back(std::vector<Node>{});
            for (auto y = 0; y < 3; y++) {
                nodes[x].push_back(Node(x, y));
            }
        }

        for (auto x = 0; x < 3; x++) {
            for (auto y = 0; y < 3; y++) {
                if (y >= 1) {
                    nodes[x][y].add_adjacent(&nodes[x][y-1]);
                }
                if (x < 2) {
                    nodes[x][y].add_adjacent(&nodes[x+1][y]);
                }
                if (y < 2) {
                    nodes[x][y].add_adjacent(&nodes[x][y+1]);
                }
                if (x < 2 && y < 2) {
                    nodes[x][y].add_adjacent(&nodes[x+1][y+1]);
                }
                
            }
        }
    }

    virtual void TearDown() {
        
    }
};

TEST_F(NodeTest, SetValueCovered) {
    nodes[1][1].set_value(Minesweeper::COVERED);
    EXPECT_EQ(nodes[1][1].value(), Minesweeper::COVERED);
}

TEST_F(NodeTest, SetValueFlag) {
    nodes[1][1].set_value(Minesweeper::FLAG);
    EXPECT_EQ(nodes[1][1].value(), Minesweeper::FLAG);
    EXPECT_EQ(nodes[1][1].mine_probability(), 1);
}

TEST_F(NodeTest, SetValueNum) {
    nodes[1][1].set_value(3);
    EXPECT_EQ(nodes[1][1].value(), 3);
    EXPECT_EQ(nodes[1][1].mine_probability(), 0);
    EXPECT_EQ(nodes[1][1].adjacent_mines_left(), 3);
}

TEST_F(NodeTest, SetValueFlagAdjacentNum) {
    nodes[0][0].set_value(2);
    nodes[2][1].set_value(4);

    EXPECT_EQ(nodes[0][0].adjacent_mines_left(), 2);
    EXPECT_EQ(nodes[2][1].adjacent_mines_left(), 4);

    nodes[1][1].set_value(Minesweeper::FLAG);

    EXPECT_EQ(nodes[0][0].adjacent_mines_left(), 1);
    EXPECT_EQ(nodes[2][1].adjacent_mines_left(), 3);
}

TEST_F(NodeTest, SetValueNumAdjacentFlag) {
    nodes[0][0].set_value(Minesweeper::FLAG);
    nodes[2][1].set_value(Minesweeper::FLAG);
    nodes[1][1].set_value(3);

    EXPECT_EQ(nodes[1][1].adjacent_mines_left(), 1);
}