#include <solver/sle.hpp>

namespace minesweeper::solver::sle {
    SystemOfLinearEquations::SystemOfLinearEquations() {

    }

    void SystemOfLinearEquations::add_equation(std::unordered_map<unsigned int, fraction> coefficients, fraction total) {
        equations.emplace_back(coefficients, total);
    }

    void SystemOfLinearEquations::add_equal(unsigned int key1, unsigned int key2) {
        std::unordered_map<unsigned int, fraction> equal({
            { key1, 1},
            { key2, -1}
        });
        add_equation(equal, 0);
    }

    void SystemOfLinearEquations::set_equal(std::set<unsigned int> equal_set) {
        auto chosen = *equal_set.begin();
        equal_set.erase(chosen);
        for (auto key : equal_set) {
            add_equal(chosen, key);
        }
    }

    void SystemOfLinearEquations::set_variable(unsigned int key, fraction value) {
        for (auto &[coefficients, total] : equations) {
            if (coefficients.contains(key)) {
                auto coeff = coefficients[key];
                coefficients.erase(key);

                total -= coeff * value;
            }
        }
    }

    void SystemOfLinearEquations::print() {
        printf("%d\n", equations.size());
        for (auto &[coefficients, total] : equations) {
            for (auto &[key, coeff] : coefficients) {
                printf("%.2f(%d) + ", boost::rational_cast<double>(coeff), key);
            }
            printf("= %.2f\n", boost::rational_cast<double>(total));
        }
        printf("\n");
    }

    void SystemOfLinearEquations::deduplicate() {
        std::vector<unsigned int> to_remove;
        for (auto i = 0; i < equations.size(); i++) {
            for (auto j = i+1; j < equations.size(); j++) {
                if (equations[i] == equations[j]) {
                    to_remove.push_back(j);
                }
            }
        }

        for (auto idx : to_remove) {
            equations.erase(equations.begin()+idx);
        }
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

    Equation SystemOfLinearEquations::scale_equation(Equation equation, fraction scale) {
        auto &[coefficients, total] = equation;
        total *= scale;
        for (auto &[key, coeff] : coefficients) {
            coeff *= scale;
        }

        return equation;
    }

    void SystemOfLinearEquations::convert_row_echelon() {
        for (auto i = 0; i < equations.size(); i++) {
            auto coefficients = equations[i].first;
            if (coefficients.size() > 0) {
                auto &[key, coeff] = *coefficients.begin();
                if (coeff < 0) {
                    equations[i] = scale_equation(equations[i], -1);
                    coeff *= -1;
                }
                for (auto j = i+1; j < equations.size(); j++) {
                    if (equations[j].first.contains(key)) {
                        auto scale = equations[j].first[key] / coeff;
                        auto scaled = scale_equation(equations[i], scale);
                        equations[j] = subtract_equations(equations[j], scaled);
                    }
                }
            }
        }
    }

    bool can_evaluate(std::unordered_map<unsigned int, fraction> coefficients, std::unordered_map<unsigned int, fraction> assignments) {
        auto unknown_variable_seen = false;
        for (auto [key, value] : coefficients) {
            if (!assignments.contains(key)) {
                if (unknown_variable_seen) {
                    return false;
                } else {
                    unknown_variable_seen = true;
                }
            }
        }
        return unknown_variable_seen;
    }

    std::pair<std::unordered_map<unsigned int, fraction>, bool> SystemOfLinearEquations::evaluate(unsigned int key, fraction value) {
        std::unordered_map<unsigned int, fraction> assignments({{key, value}});
        printf("(%d)=%0.2f\n", key, boost::rational_cast<double>(value));

        bool no_new;
        do {
        no_new = true;
            for (auto [coefficients, total]: equations) {
                if (can_evaluate(coefficients, assignments)) {
                    unsigned int new_key;
                    for (auto [key, coeff] : coefficients) {
                        if (assignments.contains(key)) {
                            total -= coeff * assignments[key];
                        } else {
                            new_key = key;
                        }
                    }
                    auto value = total / coefficients[new_key];
                    assignments.insert({new_key, value});
                    no_new = false;
                    printf("(%d)=%0.2f\n", new_key, boost::rational_cast<double>(value));
                    if (value <= 0 || value >= 1) {
                        return {assignments, false};
                    }
                }
            }
        } while (!no_new);

        return {assignments, true};
    }

    std::unordered_map<unsigned int, fraction> SystemOfLinearEquations::attempt_solve(fraction step) {
        print();

        convert_row_echelon();

        print();

        unsigned int key;
        fraction value;
        for (auto &[coefficients, total] : equations) {
            auto num_vars = coefficients.size();
            if (num_vars > 0 && num_vars <= 2) {
                key = coefficients.begin()->first;
                
                if (num_vars == 1) {
                    value = total / coefficients.begin()->second;
                    break;
                } else {
                    value = step;
                }
            }
        }

        std::unordered_map<unsigned int, fraction> assignments;
        bool valid;
        do {
            if (value >= 1) {
                throw std::runtime_error("Couldn't find a solution!");
            }
            auto result = evaluate(key, value);
            assignments = result.first;
            valid = result.second;
            value += step;
            printf("\n");
        } while (!valid && value < 1);

        return assignments;
    }

    unsigned int SystemOfLinearEquations::variable_count() {
        std::set<unsigned int> variables;
        for (auto &[coefficients, total] : equations) {
            for (auto &[key, coeff]: coefficients) {
                variables.insert(key);
            }
        }

        return variables.size();
    }
}