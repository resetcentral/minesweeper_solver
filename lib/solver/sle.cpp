#include <solver/sle.hpp>
#include <queue>

namespace minesweeper::solver::sle {
    void SystemOfLinearEquations::add_equation(Coefficients coefficients, Fraction total) {
        equations.emplace_back(coefficients, total);
    }

    void SystemOfLinearEquations::add_equal(Node* var1, Node* var2) {
        Coefficients equal({
            { var1, 1},
            { var2, -1}
        });
        add_equation(equal, 0);
    }

    void SystemOfLinearEquations::print() {
        printf("Number of Equations: %zu\n", equations.size());
        for (auto &[coefficients, total] : equations) {
            for (auto &[key, coeff] : coefficients) {
                printf("%.2f(%p) + ", boost::rational_cast<double>(coeff), key);
            }
            printf("= %.2f\n", boost::rational_cast<double>(total));
        }
        printf("\n");
    }

    Equation SystemOfLinearEquations::subtract_equations(Equation left, Equation right) {
        auto &[left_coefficients, left_total] = left;
        auto &[right_coefficients, right_total] = right;

        left_total -= right_total;
        for (auto [r_key, r_coeff] : right_coefficients) {
            if (left_coefficients.contains(r_key)) {
                auto &l_coeff = left_coefficients[r_key];
                if (l_coeff == r_coeff) {
                    left_coefficients.erase(r_key);
                } else {
                    l_coeff -= r_coeff;
                }
            } else {
                left_coefficients.insert({r_key, -r_coeff});
            }
        }
        return left;
    }

    Equation SystemOfLinearEquations::scale_equation(Equation equation, Fraction scale) {
        auto &[coefficients, total] = equation;
        for (auto &[key, coeff] : coefficients) {
            coeff *= scale;
        }
        total *= scale;

        return {coefficients, total};
    }

    void SystemOfLinearEquations::convert_row_echelon() {
        for (auto i = 0; i < equations.size(); i++) {
            auto &[eq1_coefficients, total] = equations[i];
            if (eq1_coefficients.size() > 0) {
                auto &[var, coeff] = *eq1_coefficients.begin();
                if (coeff < 0) {
                    coeff *= -1;
                    total *= -1;
                    equations[i] = scale_equation(equations[i], -1);

                }
                for (auto j = i+1; j < equations.size(); j++) {
                    auto eq2_coefficients = equations[j].first;
                    if (eq2_coefficients.contains(var)) {
                        auto scale = eq2_coefficients[var] / coeff;
                        auto scaled = scale_equation(equations[i], scale);
                        equations[j] = subtract_equations(equations[j], scaled);
                    }
                }
            }
        }
    }

    unsigned int unknown_variables(Coefficients coefficients, Assignments assignments) {
        auto unknown_variables_seen = 0;
        for (auto [key, value] : coefficients) {
            if (!assignments.contains(key)) {
                unknown_variables_seen++;
            }
        }
        return unknown_variables_seen;
    }

    std::set<Node*> SystemOfLinearEquations::variables() {
        std::set<Node*> vars;
        for (auto &[coefficients, total] : equations) {
            for (auto &[key, coeff] : coefficients) {
                vars.insert(key);
            }
        }

        return vars;
    }

    std::set<Node*> SystemOfLinearEquations::independent_variables() {
        convert_row_echelon();

        auto vars = variables();
        for (auto &[coefficients, total] : equations) {
            vars.erase(coefficients.begin()->first);
        }
        return vars;
    }

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
            throw std::invalid_argument("Cannot solve");
        }
    }

    void SystemOfLinearEquations::evaluate(Assignments& assignments) {
        std::queue<Equation> sub_equations;
        for (auto i = 0; i < equations.size(); i++) {
            sub_equations.push(equations[i]);
        }

        while (!sub_equations.empty()) {
            auto equation = sub_equations.front();
            sub_equations.pop();

            auto &[coefficients, total] = equation;
            switch (unknown_variables(coefficients, assignments)) {
                case 0: {
                    break;
                }
                case 1: {
                    auto [key, value] = solve(equation, assignments);
                    assignments.insert({key, value});
                    break;
                }
                default:
                    sub_equations.push(equation);
                    break;
            }
        }
    }
}