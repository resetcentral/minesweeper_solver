#include <solver/node.hpp>
#include <boost/rational.hpp>
#include <unordered_map>
#include <map>

namespace minesweeper::solver::sle {
    // Integer based rational
    using Fraction = boost::rational<int>;

    // map of variables to coefficients. e.g. 3/2 * x, -2 * y, etc.
    using Coefficients = std::map<Node*, Fraction>;

    // map of variable to value assignments. e.g. x = 2, y = 3/4
    using Assignments = std::unordered_map<Node*, Fraction>;

    // pair representing sum(Coefficients) = Fraction. e.g. 3/2x + -2y = 5/3
    using Equation = std::pair<Coefficients, Fraction>;
    
    /**
     * A system of linear equations.
     * 
     * Each equation in the system is the sum of a set of variable and
     * coefficient pairs which is equal to a rational number total. Each
     * variable has no exponent (i.e. x^1 not x^2) and each coefficient is a
     * rational number.
     */
    class SystemOfLinearEquations {
        std::vector<Equation> _equations;
        static unsigned int unknown_variables_count(const Coefficients& coefficients, const Assignments& assignments);
        static Equation subtract_equations(Equation left, const Equation& right);
        static Equation scale_equation(Equation equation, const Fraction& scale);

    public:
        /**
         * Solve the given Equation for the unknown variable using the
         * given Assignments. The unknown variable is the single variable
         * in `equation` which is not in `assignments`
         * 
         * @param equation Equation to solve
         * @param assignments variable Assignments to solve `equation` with
         * 
         * @return pair of the unknown variable and its value
         * 
         * @throws std::invalid_argument thrown if the number of unknown
         *      variables is not one
         */
        static std::pair<Node*, Fraction> solve(Equation equation, const Assignments& assignments);

        /**
         * Get all equations in the system of linear equations.
         * 
         * @return list of Equations
         */
        std::vector<Equation> equations() const;

        /**
         * Get all variables in the system of linear equations.
         * 
         * @return Set of all variables
         */
        std::set<Node*> variables() const;

        /**
         * Get all independent variables in the system of linear equations.
         * Independent variables are those which don't depend on the values
         * of any other variables.
         * 
         * @return Set of all independent variables
         */
        std::set<Node*> independent_variables();

        /**
         * Add an equation with the given variable Coefficients and total
         * to the system of linear equations.
         * 
         * @param coefficients map of variables to their coefficients
         * @param total value that the variables and coefficients sum to
         */
        void add_equation(const Coefficients coefficients, const Fraction total);

        /**
         * Convert the system of linear equations into row echelon form.
         * In row echelon form, each equation contains at least one variable
         * which does not exist in any of the equations after it.
         */
        void convert_row_echelon();

        /**
         * Evaluate the system of linear equations using the given Assignments and
         * add the resulting assignments of dependent variables to `assignments.
         * 
         * @param assigments map of assignments for independent variables. New
         *      assignments for dependent variables are added to this map
         * 
         * @throws std::invalid_argument thrown if there are not enough
         *      variable assignments in `assigments` to solve the system
         *      of linear equations.
         */
        void evaluate(Assignments& assignments) const;

        /**
         * Print the equations in the system of linear equations.
         */
        void print() const;
    };
}