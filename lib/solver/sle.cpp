#include <solver/sle.hpp>
#include <queue>
#include <iostream>

namespace minesweeper::solver::sle {
    // SystemOfLinearEquations implementation
    // private methods:

    /**
     * Count the number of unknown variables in `coefficients` given `assignments`.
     * 
     * @param coefficients the variables and coefficients of an Equation
     * @param assignments a set of variable->value assignments
     * 
     * @return the number of variables in `coefficients` that are not in `assignments`
     */
    unsigned int SystemOfLinearEquations::unknown_variables_count(const Coefficients& coefficients, const Assignments& assignments) {
        auto unknown_variables_seen = 0;
        for (auto [key, value] : coefficients) {
            if (!assignments.contains(key)) {
                unknown_variables_seen++;
            }
        }
        return unknown_variables_seen;
    }

    /**
     * Subtract equation `right` from equation `left`.
     * 
     * @param left Equation to be subtracted from
     * @param right Equation to subtract from `left`
     * 
     * @return result of `left` - `right`
     */
    Equation SystemOfLinearEquations::subtract_equations(Equation left, const Equation& right) {
        auto &[left_coefficients, left_total] = left;
        auto &[right_coefficients, right_total] = right;

        left_total -= right_total;
        for (auto [r_var, r_coeff] : right_coefficients) {
            if (left_coefficients.contains(r_var)) {
                auto &l_coeff = left_coefficients[r_var];
                if (l_coeff == r_coeff) {
                    left_coefficients.erase(r_var);
                } else {
                    l_coeff -= r_coeff;
                }
            } else {
                left_coefficients.insert({r_var, -r_coeff});
            }
        }
        return left;
    }

    /**
     * Multiply `equation` by the scalar `right`.
     * 
     * @param equation Equation to scale
     * @param scale Fraction to scale `equation` by
     * 
     * @return `equation` * `scale`
     */
    Equation SystemOfLinearEquations::scale_equation(Equation equation, const Fraction& scale) {
        auto &[coefficients, total] = equation;
        for (auto &[_var, coeff] : coefficients) {
            coeff *= scale;
        }
        total *= scale;

        return {coefficients, total};
    }

    // public methods:
    std::pair<Node*, Fraction> SystemOfLinearEquations::solve(Equation equation, const Assignments& assignments) {
        std::pair<Node*, Fraction> solution;
        auto &[coefficients, total] = equation;
        
        for (auto &[key, value] : assignments) {
            if (coefficients.contains(key)) {
                total -= coefficients.at(key) * assignments.at(key);
                coefficients.erase(key);
            }
        }

        if (coefficients.size() == 1) {
            auto &[key, coeff] = *coefficients.begin();
            auto value = total / coeff;
            return {key, value};
        } else {
            throw std::invalid_argument("Not enough information to solve the equation");
        }
    }

    std::vector<Equation> SystemOfLinearEquations::equations() const {
        std::vector<Equation> eqs{};

        for (auto eq : _equations) {
            eqs.push_back(eq);
        }
        return eqs;
    }

    std::set<Node*> SystemOfLinearEquations::variables() const {
        std::set<Node*> vars;
        for (auto &[coefficients, total] : _equations) {
            for (auto &[key, coeff] : coefficients) {
                vars.insert(key);
            }
        }
        return vars;
    }

    std::set<Node*> SystemOfLinearEquations::independent_variables() {
        convert_row_echelon();

        // In row echelon form, the 1st variable in each equation is dependent
        auto vars = variables();
        for (auto &[coefficients, total] : _equations) {
            vars.erase(coefficients.begin()->first);
        }
        return vars;
    }

    void SystemOfLinearEquations::add_equation(Coefficients coefficients, Fraction total) {
        _equations.emplace_back(coefficients, total);
    }

    void SystemOfLinearEquations::convert_row_echelon() {
        for (auto i = 0; i < _equations.size(); i++) {
            auto &[eq1_coefficients, total] = _equations[i];
            if (eq1_coefficients.size() > 0) {
                auto &[var, coeff] = *eq1_coefficients.begin();

                /* Subtract a scaled version of equation[i] from every equation
                after it that includes `var` */
                for (auto j = i+1; j < _equations.size(); j++) {
                    auto eq2_coefficients = _equations[j].first;
                    if (eq2_coefficients.contains(var)) {
                        auto scale = eq2_coefficients[var] / coeff;
                        auto scaled = scale_equation(_equations[i], scale);
                        _equations[j] = subtract_equations(_equations[j], scaled);
                    }
                }
            }
        }
    }

    void SystemOfLinearEquations::evaluate(Assignments& assignments) const {
        std::queue<Equation> sub_equations;
        for (auto i = 0; i < _equations.size(); i++) {
            sub_equations.push(_equations[i]);
        }

        while (!sub_equations.empty()) {
            auto equation = sub_equations.front();
            sub_equations.pop();

            auto &[coefficients, total] = equation;
            switch (unknown_variables_count(coefficients, assignments)) {
                case 0:
                    break;
                case 1: {
                    auto [key, value] = solve(equation, assignments);
                    assignments.insert({key, value});
                }
                    break;
                default: {
                    sub_equations.push(equation);
                }
                    break;
            }
        }
    }

    void SystemOfLinearEquations::print() const {
        std::cout << "Number of Equations: " << _equations.size() << "\n";
        for (auto &[coefficients, total] : _equations) {
            for (auto &[key, coeff] : coefficients) {
                std::cout << coeff << "(" << key << ")";
            }
            std::cout << "= " << total << "\n";
        }
        std::cout << std::endl;
    }
}