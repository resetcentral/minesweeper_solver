#include <solver/node.hpp>
#include <boost/rational.hpp>
#include <unordered_map>
#include <map>

namespace minesweeper::solver::sle {
    using Fraction = boost::rational<int>;
    using Coefficients = std::map<Node*, Fraction>;
    using Assignments = std::unordered_map<Node*, Fraction>;
    using Equation = std::pair<Coefficients, Fraction>;
    

    class SystemOfLinearEquations {
        std::vector<Equation> equations;

        static Equation subtract_equations(Equation left, Equation right);

        static Equation scale_equation(Equation equation, Fraction scale);

    public:
        std::set<Node*> variables();

        std::set<Node*> independent_variables();

        void add_equation(Coefficients coefficients, Fraction total);

        void add_equal(Node* var1, Node* var2);

        void convert_row_echelon();
        
        std::pair<Node*, Fraction> solve(Equation equation, const Assignments& assignments);

        void evaluate(Assignments& assignments);

        void print();
    };
}