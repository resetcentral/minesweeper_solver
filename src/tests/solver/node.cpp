#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver/node.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::Pair;
using namespace::minesweeper::solver;

TEST(SolverNode, Coord) {
    auto node = Node(3, 6);

    std::pair<unsigned int, unsigned int> expected{ 3, 6 };
    EXPECT_EQ(node.coord(), expected);
}

TEST(SolverNode, AddAdjacent) {
    auto node1 = Node(3, 6);
    auto node2 = Node(4, 6);
    
    node1.add_adjacent(&node2);
    EXPECT_THAT(node1.adjacent(), UnorderedElementsAre(&node2));
    EXPECT_THAT(node2.adjacent(), UnorderedElementsAre(&node1));
}

TEST(SolverNode, SetMineProbability) {
    auto node1 = Node(3, 6);
    Fraction prob{1, 2};
    node1.set_mine_probability(prob);

    EXPECT_EQ(node1.mine_probability(), prob);
}

TEST(SolverNode, SetMineProbabilityThrowHigh) {
    auto node1 = Node(3, 6);
    Fraction prob{4, 3};

    EXPECT_THROW(node1.set_mine_probability(prob), std::invalid_argument);
}

class NodeTest : public ::testing::Test {
    protected:
    std::vector<std::vector<Node*>> nodes;

    virtual void SetUp() {
        nodes.resize(3);
        for (auto x = 0; x < 3; x++) {
            for (auto y = 0; y < 3; y++) {
                nodes[x].push_back(new Node(x, y));
            }
        }

        for (auto x = 0; x < 3; x++) {
            for (auto y = 0; y < 3; y++) {
                if (y >= 1) {
                    nodes[x][y]->add_adjacent(nodes[x][y-1]);
                }
                if (x < 2 && y >= 1) {
                    nodes[x][y]->add_adjacent(nodes[x+1][y-1]);
                }
                if (x < 2) {
                    nodes[x][y]->add_adjacent(nodes[x+1][y]);
                }
                if (y < 2) {
                    nodes[x][y]->add_adjacent(nodes[x][y+1]);
                }
                if (x < 2 && y < 2) {
                    nodes[x][y]->add_adjacent(nodes[x+1][y+1]);
                }
                
            }
        }
    }

    virtual void TearDown() {
        for (auto column : nodes) {
            for (auto node: column) {
                delete node;
            }
        }
    }
};

TEST_F(NodeTest, Setup) {
    EXPECT_EQ(nodes[0][0]->adjacent().size(), 3);
    EXPECT_EQ(nodes[0][1]->adjacent().size(), 5);
    EXPECT_EQ(nodes[0][2]->adjacent().size(), 3);
    EXPECT_EQ(nodes[1][0]->adjacent().size(), 5);
    EXPECT_EQ(nodes[1][1]->adjacent().size(), 8);
    EXPECT_EQ(nodes[1][2]->adjacent().size(), 5);
    EXPECT_EQ(nodes[2][0]->adjacent().size(), 3);
    EXPECT_EQ(nodes[2][1]->adjacent().size(), 5);
    EXPECT_EQ(nodes[2][2]->adjacent().size(), 3);
}

TEST_F(NodeTest, SetValueCovered) {
    nodes[1][1]->set_value(Tile::Covered);
    EXPECT_EQ(nodes[1][1]->value(), Tile::Covered);
}

TEST_F(NodeTest, SetValueFlag) {
    nodes[1][1]->set_value(Tile::Flag);
    EXPECT_EQ(nodes[1][1]->value(), Tile::Flag);
    EXPECT_EQ(nodes[1][1]->mine_probability(), 1);
}

TEST_F(NodeTest, SetValueNum) {
    nodes[1][1]->set_value(Tile(3));
    EXPECT_EQ(nodes[1][1]->value(), 3);
    EXPECT_EQ(nodes[1][1]->mine_probability(), 0);
    EXPECT_EQ(nodes[1][1]->adjacent_mines_left(), 3);
}

TEST_F(NodeTest, SetValueIsHint) {
    EXPECT_FALSE(nodes[1][1]->is_hint());
    nodes[1][1]->set_value(Tile(3));
    EXPECT_TRUE(nodes[1][1]->is_hint());
}

TEST_F(NodeTest, SetValueFlagAdjacentNum) {
    nodes[0][0]->set_value(Tile(2));
    nodes[2][1]->set_value(Tile(4));

    EXPECT_EQ(nodes[0][0]->adjacent_mines_left(), 2);
    EXPECT_EQ(nodes[2][1]->adjacent_mines_left(), 4);

    nodes[1][1]->set_value(Tile::Flag);

    EXPECT_EQ(nodes[0][0]->adjacent_mines_left(), 1);
    EXPECT_EQ(nodes[2][1]->adjacent_mines_left(), 3);
}

TEST_F(NodeTest, SetValueNumAdjacentFlag) {
    nodes[0][0]->set_value(Tile::Flag);
    nodes[2][1]->set_value(Tile::Flag);
    nodes[1][1]->set_value(Tile(3));

    EXPECT_EQ(nodes[1][1]->adjacent_mines_left(), 1);
}

TEST_F(NodeTest, AdjacentCovered) {
    nodes[1][0]->set_value(Tile(3));
    nodes[0][1]->set_value(Tile(2));
    nodes[1][2]->set_value(Tile(5));

    EXPECT_THAT(nodes[0][0]->adjacent_covered(), 
        UnorderedElementsAre(nodes[1][1]));

    EXPECT_THAT(nodes[2][0]->adjacent_covered(), 
        UnorderedElementsAre(nodes[1][1], nodes[2][1]));

    EXPECT_THAT(nodes[1][1]->adjacent_covered(), 
        UnorderedElementsAre(nodes[0][0], nodes[0][2], nodes[2][0], nodes[2][1], nodes[2][2]));
}

TEST_F(NodeTest, AdjacentCoveredCount) {
    nodes[1][0]->set_value(Tile(3));
    nodes[0][1]->set_value(Tile(2));
    nodes[1][2]->set_value(Tile(5));

    EXPECT_EQ(nodes[0][0]->adjacent_covered_count(), 1);
    EXPECT_EQ(nodes[2][0]->adjacent_covered_count(), 2);
    EXPECT_EQ(nodes[1][1]->adjacent_covered_count(), 5);
}

TEST_F(NodeTest, CoveredEdge) {
    nodes[0][0]->set_value(Tile(1));
    nodes[0][1]->set_value(Tile(1));
    nodes[1][0]->set_value(Tile(1));
    
    EXPECT_FALSE(nodes[0][0]->covered_edge());
    EXPECT_FALSE(nodes[0][1]->covered_edge());
    EXPECT_TRUE(nodes[0][2]->covered_edge());
    EXPECT_FALSE(nodes[1][0]->covered_edge());
    EXPECT_TRUE(nodes[1][1]->covered_edge());
    EXPECT_TRUE(nodes[1][2]->covered_edge());
    EXPECT_TRUE(nodes[2][0]->covered_edge());
    EXPECT_TRUE(nodes[2][1]->covered_edge());
    EXPECT_FALSE(nodes[2][2]->covered_edge());
}

TEST_F(NodeTest, NumberEdge) {
    nodes[0][0]->set_value(Tile(1));
    nodes[0][1]->set_value(Tile(1));
    nodes[1][0]->set_value(Tile(1));
    nodes[1][1]->set_value(Tile(1));
    
    EXPECT_FALSE(nodes[0][0]->hint_edge());
    EXPECT_TRUE(nodes[0][1]->hint_edge());
    EXPECT_FALSE(nodes[0][2]->hint_edge());
    EXPECT_TRUE(nodes[1][0]->hint_edge());
    EXPECT_TRUE(nodes[1][1]->hint_edge());
    EXPECT_FALSE(nodes[1][2]->hint_edge());
    EXPECT_FALSE(nodes[2][0]->hint_edge());
    EXPECT_FALSE(nodes[2][1]->hint_edge());
    EXPECT_FALSE(nodes[2][2]->hint_edge());
}

TEST_F(NodeTest, CoveredSafe) {
    /*
     * 2 f O
     * O O 1
     * O O O
    */
    nodes[0][0]->set_value(Tile(2));
    nodes[2][1]->set_value(Tile(1));
    nodes[1][0]->set_value(Tile::Flag);

    EXPECT_FALSE(nodes[0][0]->covered_safe());
    EXPECT_FALSE(nodes[0][1]->covered_safe());
    EXPECT_FALSE(nodes[0][2]->covered_safe());
    EXPECT_FALSE(nodes[1][0]->covered_safe());
    EXPECT_TRUE(nodes[1][1]->covered_safe());
    EXPECT_TRUE(nodes[1][2]->covered_safe());
    EXPECT_TRUE(nodes[2][0]->covered_safe());
    EXPECT_FALSE(nodes[2][1]->covered_safe());
    EXPECT_TRUE(nodes[2][2]->covered_safe());
}