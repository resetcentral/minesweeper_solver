#include <set_utils.hpp>
#include <solver/solver.hpp>
#include <stdexcept>
#include <chrono>
#include <thread>
#include "stdlib.h"
#include <iterator>
#include <unordered_map>
#include <map>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    using Fraction = boost::rational<int>;

    SolverState::SolverState(const Minefield& minefield) {
        if (minefield.size() == 0 || minefield[0].size() == 0) {
            throw std::invalid_argument("Solver state cannot have zero width or height.");
        }
        
        auto width = minefield.size();
        auto height = minefield[0].size();

        // Set node values
        state.resize(width);
        for (auto x = 0; x < width; x++) {
            state[x].reserve(height);
            for (auto y = 0; y < height; y++) {
                auto n = new Node(x, y);
                state[x].push_back(n);
            }
        }

        // Link adjacent nodes
        for (auto x = 0; x < width; x++) {
            bool link_left = x > 0;
            for (auto y = 0; y < height; y++) {
                bool link_right = x < width-1;
                bool link_down = y < height-1;
                if (link_right) {
                    state[x][y]->add_adjacent(state[x+1][y]);
                }
                if (link_down) {
                    state[x][y]->add_adjacent(state[x][y+1]);
                }
                if (link_right && link_down) {
                    state[x][y]->add_adjacent(state[x+1][y+1]);
                }
                if (link_left && link_down) {
                    state[x][y]->add_adjacent(state[x-1][y+1]);
                }
            }
        }
        for (auto x = 0; x < width; x++) {
            for (auto y = 0; y < height; y++) {
                state[x][y]->set_value(minefield[x][y]);
            }
        }
    }

    unsigned int SolverState::width() const {
        return state.size();
    }

    unsigned int SolverState::height() const {
        return state[0].size();
    }

    std::set<Node*> SolverState::covered() {
        std::set<Node*> covered;
        for (auto x = 0; x < state.size(); x++) {
            for (auto y = 0; y < state[x].size(); y++) {
                if (state[x][y]->value() == Tile::Covered) {
                    covered.insert(state[x][y]);
                }
            }
        }
        return covered;
    }

    std::set<Node*> SolverState::hint_edge() {
        std::set<Node*> edge_set;
        for (auto x = 0; x < state.size(); x++) {
            for (auto y = 0; y < state[x].size(); y++) {
                if (state[x][y]->hint_edge()) {
                    edge_set.insert(state[x][y]);
                }
            }
        }
        return edge_set;
    }

    std::set<Node*> SolverState::covered_edge() {
        std::set<Node*> edge_set;
        for (auto x = 0; x < state.size(); x++) {
            for (auto y = 0; y < state[x].size(); y++) {
                if (state[x][y]->covered_edge()) {
                    edge_set.insert(state[x][y]);
                }
            }
        }
        return edge_set;
    }

    void SolverState::update(Node* node, const minesweeper::Minefield& minefield) {
        auto [x, y] = node->coord();
        auto value = minefield[x][y];

        node->set_value(value);
        if (value == 0) {
            for (auto adjacent_tile : node->adjacent()) {
                if (adjacent_tile->value() == Tile::Covered) {
                    update(adjacent_tile, minefield);
                }
            }
        }
    }

    const Node& SolverState::selected() const {
        return *_selected;
    }

    void SolverState::set_selected(Node* node) {
        _selected = node;
    }

    Node* SolverState::get_node(const unsigned int x, const unsigned int y) {
        return state[x][y];
    }

    // State logger
    void StateLogger::set_mode(const char* mode) {
        _mode = mode;
    }

    void StateLogger::log(SolverState state) {
        system("clear");
        printf("Solve Mode in Use: %s\n\n", _mode);
        auto selected_coords = state.selected().coord();
        for (auto y = 0; y < state.height(); y++) {
            for (auto x = 0; x < state.width(); x++) {
                if (x == selected_coords.first && y == selected_coords.second) {
                    printf("\x1b[32m"); // Green
                }

                auto value = state.get_node(x, y)->value();
                switch (value) {
                    case Tile::Mine:
                        printf("* ");
                        break;
                    case Tile::Flag:
                        printf("f ");
                        break;
                    case Tile::Covered:
                        printf("O ");
                        break;
                    case 0:
                        printf("  ");
                        break;
                    default:
                        printf("%d ", value);
                }

                if (x == selected_coords.first && y == selected_coords.second) {
                    printf("\x1b[0m"); // Reset color
                }
            }
            printf("\n");
        }
        printf("\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    
    // BasicSolver
    std::set<Node*> BasicSolver::flaggable(SolverState state) {
        std::set<Node*> flaggable_nodes;

        auto hint_edge = state.hint_edge();
        for (auto hint : hint_edge) {
            if (hint->adjacent_covered_count() == hint->adjacent_mines_left()) {
                for (auto adj_node: hint->adjacent()) {
                    if (adj_node->value() == Tile::Covered) {
                        flaggable_nodes.insert(adj_node);
                    }
                }
            }
        }
        return flaggable_nodes;
    }

    std::set<Node*> BasicSolver::safe(SolverState state) {
        std::set<Node*> safe_nodes{};

        auto covered_edge = state.covered_edge();
        for (auto node : covered_edge) {
            if (node->covered_safe()) {
                safe_nodes.insert(node);
            }
        }
        return safe_nodes;
    }

    
    // AdvancedSolver
    std::set<Node*> AdvancedSolver::flaggable(SolverState state) {
        std::set<Node*> flag;

        auto hint_edge = state.hint_edge();
        for (auto hint : hint_edge) {
            auto node_mines = hint->adjacent_mines_left();
            auto adjacent_covered = hint->adjacent_covered();
            for (auto adj_node : adjacent_covered) {
                auto adj_hints = adj_node->adjacent_active_hints();
                for (auto adj_hint : adj_hints) {
                    auto adj_mines = adj_hint->adjacent_mines_left();
                    if (node_mines >  adj_mines) {
                        auto adj_hint_adj_covered = adj_hint->adjacent_covered();
                        auto set_diff = set_utils::set_difference(adjacent_covered, adj_hint_adj_covered);
                        auto mines_diff = node_mines - adj_mines;
                        if (set_diff.size() == mines_diff) {
                            flag.merge(set_diff);
                        }
                    }
                }
            }
        }

        return flag;
    }

    std::set<Node*> AdvancedSolver::safe(SolverState state) {
        std::set<Node*> safe_nodes;
        
        auto hint_edge = state.hint_edge();
        for (auto node : hint_edge) {
            if (node->adjacent_mines_left() == 1) {
                auto adjacent_covered = node->adjacent_covered();
                for (auto adj_node : adjacent_covered) {
                    auto adj_hints = adj_node->adjacent_active_hints();
                    for (auto adj_hint : adj_hints) {
                        auto adj_hint_adj_covered = adj_hint->adjacent_covered();
                        auto set_diff = set_utils::set_difference(adj_hint_adj_covered, adjacent_covered);
                        if (set_diff.size() < adj_hint->adjacent_mines_left()) {
                            // Any nodes not shared by the two numbers, must be safe
                            // otherwise, adj_hint cannot reach the required number of mines
                            set_diff = set_utils::set_difference(adjacent_covered, adj_hint_adj_covered);
                            safe_nodes.merge(set_diff);
                        }
                    }
                }
            }
        }

        return safe_nodes;
    }


    // ProbableSolver
    void ProbableSolver::calculate_probability(SolverState state, int mines_left) {
        // Create a system of linear equations like
        // 2 = 1a + 1b + 1c + 1d 
        // using adjacent mines and covered neighbors
        sle::SystemOfLinearEquations equations;
        auto hint_edge = state.hint_edge();
        for (auto hint : hint_edge) {
            auto mines = hint->adjacent_mines_left();
            Fraction total { mines };
            sle::Coefficients coefficients;

            auto adj_covered = hint->adjacent_covered();
            for (auto node : adj_covered) {
                coefficients.insert({ node, 1});
            }
            equations.add_equation(coefficients, total);
        }

        // TODO how to incorporate sum(all_covered) = mines_left?

        // Bruteforce the possible values for the independent variables
        auto ind_vars = equations.independent_variables();
        auto assignments = bruteforce(equations, ind_vars);

        // Set probabilities of edge nodes
        auto covered_edge_nodes = state.covered_edge();
        Fraction total_probability{};
        for (auto node : covered_edge_nodes) {
            if (assignments.contains(node)) {
                node->set_mine_probability(assignments[node]);
                total_probability += assignments[node];
            }
        }

        // Set probabilities of non-edge nodes
        auto non_edge_covered = set_utils::set_difference(state.covered(), covered_edge_nodes);
        if (non_edge_covered.size() > 0) {
            auto p = (mines_left - total_probability) / Fraction{non_edge_covered.size()};

            for (auto node : non_edge_covered) {
                if (p > 1) { // hack due to not using sum(all_covered) = mines_left
                    p = 1;
                }
                node->set_mine_probability(p);
            }
        }
    }

    bool ProbableSolver::validate_assignments(sle::Assignments assignments) {
        for (auto &[var, value] : assignments) {
            if (value < 0 || value > 1) {
                return false;
            }
        }
        return true;
    }

    sle::Assignments ProbableSolver::bruteforce(sle::SystemOfLinearEquations& sys_eq, const std::set<Node*>& ind_vars) {
        std::unordered_map<Node*, unsigned int> var_bit_map;
        
        std::unordered_map<Node*, Fraction> values_valid;

        auto i = 0;
        for (auto var : ind_vars) {
            var_bit_map.insert({var, i});
            values_valid.insert({var, 0});
            i++;
        }
        auto sample = ind_vars.size() > 10;
        printf("Calculating...\n");

        auto max = 1UL << ind_vars.size();

        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<unsigned long> sample_dist(0, max);

        auto total_valid = 0;
        auto range_max = sample ? 1UL << 10 : max;
        for (auto i = 0UL; i < range_max; i++) {
            sle::Assignments assignments;
            auto value = sample ? sample_dist(rng) : i;
            for (auto var : ind_vars) {
                assignments.insert({
                    var,
                    (i >> var_bit_map[var]) & 1
                });
            }
            sys_eq.evaluate(assignments);
            if (validate_assignments(assignments)) {
                for (auto &[var, value] : assignments) {
                    values_valid[var] += value;
                }
                total_valid++;
            }
        }
        total_valid = total_valid > 0 ? total_valid : 1;

        sle::Assignments assignments;
        for (auto &[var, value] : values_valid) {
            assignments.insert({
                var,
                value / total_valid
            });
        }
        sys_eq.evaluate(assignments);

        return assignments;
    }

    Node* ProbableSolver::solve(SolverState state, int mines_left) {
        calculate_probability(state, mines_left);

        // print_probabilities(state);

        auto covered_nodes = state.covered();
        Node* least_probable = *covered_nodes.begin();
        for (auto node : covered_nodes) {
            if (node->mine_probability() < least_probable->mine_probability()) {
                least_probable = node;
            }
        }
        return least_probable;
    }

    void ProbableSolver::print_probabilities(SolverState state) {
        printf("\n");
        for (auto y = 0; y < state.height(); y++) {
            for (auto x = 0; x < state.width(); x++) {
                auto node = state.get_node(x, y);
                if (node->value() == Tile::Covered) {
                    printf("\x1b[32m"); // Green
                }
                printf("%3d/%d", node->mine_probability().numerator(), node->mine_probability().denominator());
                printf("\x1b[0m"); // Reset color
            }
            printf("\n");
        }
        printf("\n");
    }

    
    // Main solver
    MinesweeperSolver::MinesweeperSolver(Minesweeper game, StateLogger logger)
        : game { game },
          state { SolverState(game.get_field()) },
          logger { logger } {}

    void MinesweeperSolver::flag_or_uncover(Node* node, bool flag) {
        Minesweeper::GameState game_state;

        state.set_selected(node);
        logger.log(state);

        auto [x, y] = node->coord();
        if (flag) {
            game.toggle_flag(x, y);
            game_state = Minesweeper::GameState::Continue;
        } else {
            game_state = game.uncover_tile(x, y);
        }
        state.update(node, game.get_field());
        logger.log(state);

        check_game_state(game_state);
    }

    void MinesweeperSolver::check_game_state(Minesweeper::GameState game_state) {
        switch (game_state) {
            case Minesweeper::GameState::Lose: {
                throw std::logic_error("Oops! Clicked on a mine");
                break;
            }
            case Minesweeper::GameState::Win: {
                for (auto node : state.covered()) {
                    flag_or_uncover(node, true);
                }
                printf("Minefield Swept!\n");
                exit(0);
            }
            default:
                return;
        }
    }

    void MinesweeperSolver::flag_all(std::set<Node*> nodes) {
        for (auto node : nodes) {
            flag_or_uncover(node, true);
        }
    }

    void MinesweeperSolver::uncover_all(std::set<Node*> nodes) {
        for (auto node : nodes) {
            flag_or_uncover(node, false);
        }
    }

    void MinesweeperSolver::solve() {
        BasicSolver basic;
        AdvancedSolver advanced;
        ProbableSolver probable;

        Minesweeper::GameState game_state;
        auto x = game.width/2;
        auto y = game.height/2;
        flag_or_uncover(state.get_node(x, y), false);

        while (true) {
            logger.set_mode("Basic");
            auto mine_nodes = basic.flaggable(state);
            flag_all(mine_nodes);

            auto safe_nodes = basic.safe(state);
            if (safe_nodes.size() > 0) {
                uncover_all(safe_nodes);
            } else {
                logger.set_mode("Advanced");
                mine_nodes = advanced.flaggable(state);
                flag_all(mine_nodes);

                safe_nodes = advanced.safe(state);
                if (safe_nodes.size() > 0) {
                    uncover_all(safe_nodes);
                } else {
                    logger.set_mode("Most probable (this might blow up)");
                    auto picked = probable.solve(state, game.mines_left());
                    flag_or_uncover(picked, false);
                }
            }
        }
    }
}