#include <set_utils.hpp>
#include <solver/solver.hpp>
#include <stdexcept>
#include <chrono>
#include <thread>
#include "stdlib.h"
#include <iterator>


namespace minesweeper::solver {
    using minesweeper::Minesweeper;

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
                if (state[x][y]->value() == Minesweeper::COVERED) {
                    covered.insert(state[x][y]);
                }
            }
        }
        return covered;
    }

    std::set<Node*> SolverState::number_edge() {
        std::set<Node*> edge_set;
        for (auto x = 0; x < state.size(); x++) {
            for (auto y = 0; y < state[x].size(); y++) {
                if (state[x][y]->number_edge()) {
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
                if (adjacent_tile->value() == Minesweeper::COVERED) {
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
                    case Minesweeper::MINE:
                        printf("* ");
                        break;
                    case Minesweeper::FLAG:
                        printf("f ");
                        break;
                    case Minesweeper::COVERED:
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
        // std::this_thread::sleep_for(std::chrono::milliseconds(250)); TODO move this
    }
    
    // BasicSolver
    std::set<Node*> BasicSolver::flaggable(SolverState state) {
        std::set<Node*> flaggable_nodes;

        auto num_edge = state.number_edge();
        for (auto node : num_edge) {
            if (node->adjacent_covered_count() == node->adjacent_mines_left()) {
                for (auto adj_node: node->adjacent()) {
                    if (adj_node->value() == Minesweeper::COVERED) {
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

    
    // Advanced Solver
    std::set<std::pair<Node*, bool>> AdvancedSolver::compare_sets(std::set<Node*> set1, int set1_mines, std::set<Node*> set2, int set2_mines) {
        std::set<std::pair<Node*, bool>> definitive;

        std::array<std::set<Node*>, 3> segments;
        segments[1] = set_utils::set_intersection(set1, set2);
        if (!segments[1].empty()) {
            segments[0] = set_utils::set_difference(set1, set2);
            segments[2] = set_utils::set_difference(set2, set1);

            std::array<std::size_t, 3> counts{};
            for (auto i = 0; i < segments.size(); i++) {
                counts[i] = segments[i].size();
            }

            if (counts[0] > 8 || counts[2] > 8) {
                return definitive;
            }

            auto running_total = 0;
            std::array<int, 3> choose{ set1_mines, 0, set2_mines };
            std::array<int, 3> picked{};
            auto choose_table = choose_matrix<8,8>();
            while (choose[0] >= 0 && choose[2] >= 0) {
                auto total = 1;
                for (auto i = 0; i < 3; i++) {
                    total *= choose_table[counts[i]][choose[i]];
                }
                for (auto i = 0; i < 3; i++) {
                    picked[i] += choose[i] * total;
                }
                running_total += total;
                choose[0]--;
                choose[1]++;
                choose[2]--;
            }

            for (auto i = 0; i < 3; i++) {
                auto segment = segments[i];
                if (picked[i] == 0) {
                    for (auto node : segment) {
                        definitive.insert(std::pair{node, false});
                    }
                } else if (counts[i] > 0) {
                    boost::rational<unsigned int> probability{ picked[i] / counts[i] , running_total };
                    for (auto node : segment) {
                        if (probability == 1) {
                            definitive.insert(std::pair{node, true});
                        } else if (node->mine_probability().numerator() == 0) {
                            node->set_mine_probability(probability);
                        } else {
                            boost::rational<unsigned int> one{1, 1};
                            auto old_probability = node->mine_probability();
                            auto num = old_probability.numerator() * probability.numerator();
                            auto den = ((one - old_probability).numerator() * (one - probability).numerator()) + num;

                            boost::rational<unsigned int> new_probability{num, den};
                            node->set_mine_probability(new_probability);
                        }
                    }
                }
            }
        }
        
        return definitive;
    }

    std::set<Node*> AdvancedSolver::flaggable(SolverState state) {
        std::set<Node*> flag;

        auto num_edge = state.number_edge();

        for (auto node : num_edge) {
            auto node_mines = node->adjacent_mines_left();
            auto adjacent_covered = node->adjacent_covered();
            for (auto adj_node : adjacent_covered) {
                auto adj_nums = adj_node->adjacent_active_numbers();
                for (auto adj_num : adj_nums) {
                    auto adj_mines = adj_num->adjacent_mines_left();
                    if (node_mines >  adj_mines) {
                        auto adj_num_adj_covered = adj_num->adjacent_covered();
                        auto set_diff = set_utils::set_difference(adjacent_covered, adj_num_adj_covered);
                        auto mines_diff = node_mines - adj_mines;
                        if (set_diff.size() == mines_diff) {
                            flag.merge(set_diff);
                        } else if (set_diff.size() < mines_diff) {
                            printf("Node (%d, %d): %d and Node (%d, %d): %d cannot coexist\n",
                                node->coord().first,
                                node->coord().second,
                                node->value(),
                                adj_num->coord().first,
                                adj_num->coord().second,
                                adj_num->value()
                            );
                            throw std::logic_error("Encountered impossible situation");
                        }
                    }
                }
            }
        }

        return flag;
    }

    std::set<Node*> AdvancedSolver::safe(SolverState state) {
        std::set<Node*> safe_nodes;
        
        auto num_edge = state.number_edge();
        for (auto node : num_edge) {
            if (node->adjacent_mines_left() == 1) {
                auto adjacent_covered = node->adjacent_covered();
                for (auto adj_node : adjacent_covered) {
                    auto adj_nums = adj_node->adjacent_active_numbers();
                    for (auto adj_num : adj_nums) {
                        auto adj_num_adj_covered = adj_num->adjacent_covered();
                        auto set_diff = set_utils::set_difference(adj_num_adj_covered, adjacent_covered);
                        if (set_diff.size() < adj_num->adjacent_mines_left()) {
                            // Any nodes not shared by the two numbers, must be safe
                            // otherwise, adj_num cannot reach the required number of mines
                            set_diff = set_utils::set_difference(adjacent_covered, adj_num_adj_covered);
                            safe_nodes.merge(set_diff);
                        }
                    }
                }
            }
        }

        return safe_nodes;
    }

    std::set<std::pair<Node*, bool>> AdvancedSolver::solve(SolverState state, int mines_left) {
        std::set<std::pair<Node*, bool>> results;

        auto num_edge = state.number_edge();
        for (auto node1 : num_edge) {
            for (auto node2 : num_edge) {
                if (node1 == node2) {
                    continue;
                }

                auto definitive = compare_sets(node1->adjacent_covered(),
                    node1->adjacent_mines_left(),
                    node2->adjacent_covered(),
                    node2->adjacent_mines_left()
                );
                results.merge(definitive);
                if (definitive.size() > 0) {
                    return definitive;
                }
            }
            // definitive = compare_sets(
            //     node1->adjacent_covered(),
            //     node1->adjacent_mines_left(),
            //     state.covered(),
            //     mines_left
            // );
            // if (definitive.size() > 0) {
            //     throw std::logic_error("How did we get here?");
            //     return definitive;
            // }
        }
        auto covered_edge = state.covered_edge();
        for (auto node : num_edge) {
            auto covered = node->adjacent_covered();

        }

        return results;
    }


    // Least probable
    void ProbableSolver::calculate_probability(SolverState state, int mines_left) {
        auto covered_edge_nodes = state.covered_edge();
        auto covered_nodes = state.covered();

        auto number_edge_nodes = state.number_edge();
        if (number_edge_nodes.size() == 1) {
            boost::rational<unsigned int> probability{
                (*number_edge_nodes.begin())->adjacent_mines_left(),
                covered_edge_nodes.size()
            };
            for (auto node : covered_edge_nodes) {
                node->set_mine_probability(probability);
            }
        };

        boost::rational<unsigned int> edge_probability {};
        for (auto node : covered_edge_nodes) {
            edge_probability += node->mine_probability();
        }

        auto non_edge_sum = boost::rational<unsigned int>{ mines_left } - edge_probability;
        auto non_edge_covered_count = covered_nodes.size() - covered_edge_nodes.size();
        if (non_edge_covered_count > 0) {
            auto non_edge_probability = non_edge_sum / boost::rational<unsigned int>{ non_edge_covered_count };
            for (auto node : covered_nodes) {
                if (!covered_edge_nodes.contains(node)) {
                    node->set_mine_probability(non_edge_probability);
                }
            }
        }
    }

    Node* ProbableSolver::solve(SolverState state, int mines_left) {
        calculate_probability(state, mines_left);

        auto covered_nodes = state.covered();
        Node* least_probable = *covered_nodes.begin();
        for (auto node : covered_nodes) {
            if (node->mine_probability() < least_probable->mine_probability()) {
                least_probable = node;
            }
        }
        return least_probable;
    }

    
    // Main solver
    MinesweeperSolver::MinesweeperSolver(Minesweeper game, StateLogger logger)
        : game { game },
          state { SolverState(game.get_field()) } {

    }

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
            for (auto node : mine_nodes) {
                flag_or_uncover(node, true);
            }

            auto safe_nodes = basic.safe(state);
            if (safe_nodes.size() > 0) {
                for (auto node : safe_nodes) {
                    if (node->value() == Minesweeper::COVERED) {
                        flag_or_uncover(node, false);
                    }
                }
            } else {
                logger.set_mode("Advanced");
                auto decisions = advanced.solve(state, game.mines_left());
                if (decisions.size() > 0) {
                    for (auto [node, flag] : decisions) {
                        flag_or_uncover(node, flag);
                    }
                } else {
                    logger.set_mode("Most probable (this might blow up)");
                    auto picked = probable.solve(state, game.mines_left());
                    flag_or_uncover(picked, false);
                }
            }
        }
    }
}

// int main() {
//     // Get height, width, num mines (or give options: Beginner, intermediate, expert)

//     minesweeper::Minesweeper game(30, 16, 99);
//     minesweeper::solver::MinesweeperSolver solver(game);
//     solver.solve();
// }