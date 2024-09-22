#include <boost/rational.hpp>
#include <unordered_map>
#include <vector>
#include <set>

namespace minesweeper::solver::sle {
    using fraction = boost::rational<int>;
    using Equation = std::pair<std::unordered_map<unsigned int, fraction>, fraction>;

    class SystemOfLinearEquations {
        std::vector<Equation> equations;
        std::set<unsigned int> free_variables;

        Equation subtract_equations(Equation left, Equation right);

        Equation scale_equation(Equation equation, fraction scale);

        std::pair<std::unordered_map<unsigned int, fraction>, bool> evaluate(unsigned int key, fraction value);

        void convert_row_echelon();

        void deduplicate();

    public:
        SystemOfLinearEquations();

        void add_equation(std::unordered_map<unsigned int, fraction> coefficients, fraction total);

        void add_equal(unsigned int key1, unsigned int key2);

        void set_equal(std::set<unsigned int> equal_set);

        void set_variable(unsigned int key, fraction value);

        std::unordered_map<unsigned int, fraction> attempt_solve(fraction step);

        unsigned int variable_count();

        void print();
    };
}