#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <solver/sle.hpp>

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;

using namespace minesweeper::solver::sle;
using minesweeper::solver::Node;

class SleTest : public ::testing::Test {
    protected:
    SystemOfLinearEquations system;
    std::array<Node*, 5> nodes;

    Equation equation1, equation2, equation3, equation4;

    virtual void SetUp() {
        for (unsigned long i = 0; i < nodes.size(); i++) {
            nodes[i] = (Node*) i+1;
        }

        equation1 = Equation {
            {
                {nodes[1], 1},
                {nodes[3], 1},
                {nodes[0], 1},
            },
            2
        };

        equation2 = Equation {
            {
                {nodes[0], 2},
                {nodes[1], 3},
                {nodes[2], 1},
            },
            4
        };

        equation3 = Equation {
            {
                {nodes[3], -1},
                {nodes[1], 2},
                {nodes[2], Fraction { 3, 4 }},
            },
            Fraction { 5, 3 }
        };

        equation4 = Equation {
            {
                {nodes[0], Fraction { 3, 2 }},
                {nodes[1], 3},
                {nodes[2], Fraction { 1, 4 }},
                {nodes[3], Fraction { 2, 2 }},
                {nodes[4], Fraction { 1, 6 }},
            },
            4
        };

        system.add_equation(equation1.first, equation1.second);
        system.add_equation(equation2.first, equation2.second);
        system.add_equation(equation3.first, equation3.second);
    }

    virtual void TearDown() {

    }
};

TEST_F(SleTest, Variables) {
    auto variables = system.variables();

    EXPECT_THAT(variables, UnorderedElementsAre(
        nodes[0],
        nodes[1],
        nodes[2],
        nodes[3]
    ));
}

TEST_F(SleTest, ConvertRowEchelon) {
    system.convert_row_echelon();

    auto equations = system.equations();

    EXPECT_THAT(equations, ElementsAre(
        equation1,
        Equation{
            {
                { nodes[1], 1 }, 
                { nodes[2], 1 },
                { nodes[3], -2 }
            }, 
            0
        },
        Equation{
            {
                {nodes[2], Fraction{ -5, 4 }},
                {nodes[3], 3}
            },
            Fraction { 5, 3 }
        }
    ));
}

TEST_F(SleTest, IndependentVariables) {
    auto ind_variables = system.independent_variables();

    EXPECT_EQ(ind_variables.size(), 1);
}

TEST_F(SleTest, AddEquation) {
    system.add_equation(equation4.first, equation4.second);
    auto equations = system.equations();

    EXPECT_THAT(equations, ElementsAre(
        equation1,
        equation2,
        equation3,
        equation4
    ));
}

TEST_F(SleTest, Solve) {
    Assignments assignments{
        { nodes[0], Fraction {1, 4}},
        { nodes[2], Fraction { 2 }}
    };
    Fraction one_half{1, 2};

    auto result = SystemOfLinearEquations::solve(equation2, assignments);
    EXPECT_EQ(result.first, nodes[1]);
    EXPECT_EQ(result.second, one_half);
}

TEST_F(SleTest, SolveNoUnknownVars) {
    Equation eq {
        {
            { nodes[0], 1 },
            { nodes[1], 2 }
        },
        2
    };

    Assignments assignments {
        { nodes[0], Fraction{ 1, 2 } },
        { nodes[1], Fraction{ 3, 4 }}
    };

    EXPECT_THROW(SystemOfLinearEquations::solve(eq, assignments), std::invalid_argument);
}

TEST_F(SleTest, SolveTooManyUnknownVars) {
    Equation eq {
        {
            { nodes[0], 1 },
            { nodes[1], 2 },
            { nodes[2], 3 }
        },
        2
    };

    Assignments assignments {
        { nodes[0], Fraction{ 1, 2 } }
    };

    EXPECT_THROW(SystemOfLinearEquations::solve(eq, assignments), std::invalid_argument);
}

TEST_F(SleTest, Evaluate) {
    Assignments assignments {
        { nodes[3], Fraction{ 1, 3 } }
    };
    Assignments expected {
        {nodes[0], Fraction { 7, 15 }},
        {nodes[1], Fraction { 6, 5 }},
        {nodes[2], Fraction { -8, 15 }},
        {nodes[3], Fraction { 1, 3 }}
    };

    system.convert_row_echelon();
    system.evaluate(assignments);
    EXPECT_EQ(assignments, expected);
}