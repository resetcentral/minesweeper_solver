#include <solver.hpp>

#include <stdexcept>
#include <chrono>
#include <thread>
#include "stdlib.h"
#include <algorithm>
#include <iterator>


namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    Node::Node(unsigned int x, unsigned int y)
        : _coord { x, y } {}

    std::pair<unsigned int, unsigned int> Node::coord() {
        return _coord;
    }

    unsigned int Node::value() {
        return _value;
    }

    void Node::set_value(unsigned int value) {
        _value = value;

        if (value == Minesweeper::FLAG) {
            set_mine_probability(1);
            for (auto node : _adjacent) {
                if (node->value() <= 8) {
                    node->_adjacent_mines_left--;
                }
            }
        } else if (value <= 8) {
            set_mine_probability(0);
            _adjacent_mines_left = value;
            for (auto node : _adjacent) {
                if (node->value() == Minesweeper::FLAG) {
                    _adjacent_mines_left--;
                }
            }
        }
    }

    const std::set<Node*>& Node::adjacent() {
        return _adjacent;
    }

    std::set<Node*> Node::adjacent_covered() {
        std::set<Node*> covered;
        for (auto node : _adjacent) {
            if (node->value() == Minesweeper::COVERED) {
                covered.insert(node);
            }
        }

        return covered;
    }

    unsigned int Node::adjacent_covered_count() {
        unsigned int count = 0;
        for (auto node : _adjacent) {
            if (node->value() == Minesweeper::COVERED) {
                count++;
            }
        }

        return count;
    }

    void Node::add_adjacent(Node* node) {
        if(_adjacent.insert(node).second) {
            node->add_adjacent(this);
        }
    }

    boost::rational<unsigned int> Node::mine_probability() {
        return _mine_probability;
    }

    void Node::set_mine_probability(boost::rational<unsigned int> mp) {
        if (mp > 1) {
            throw std::invalid_argument("Mine probability cannot be less than 0 or greater than 1.");
        }

        _mine_probability = mp;
    }

    unsigned int Node::adjacent_mines_left() {
        return _adjacent_mines_left;
    }

    bool Node::edge_covered() {
        if (_value == Minesweeper::COVERED) {
            for (auto node : _adjacent) {
                if (node->value() <= 8) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::edge_num() {
        if (_value <= 8) {
            for (auto node : _adjacent) {
                if (node->value() == Minesweeper::COVERED) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::safe() {
        for (auto adj_node: _adjacent) {
            if (adj_node->value() <= 8 && adj_node->adjacent_mines_left() == 0) {
                return true;
            }
        }
        return false;
    }

    class MinesweeperSolver {
        Minesweeper game;
        std::vector<std::vector<Node>> state;
        std::pair<unsigned int, unsigned int> last_clicked;
        
        public:
        void calculate_probability() {
            auto edge_covered = get_edge_covered();
            auto covered = all_covered();

            boost::rational<unsigned int> edge_probability {};
            for (auto node : edge_covered) {
                edge_probability += node->mine_probability();
            }

            auto non_edge_sum = boost::rational<unsigned int>{ game.mines_left() } - edge_probability;
            auto non_edge_covered_count = covered.size() - edge_covered.size();
            if (non_edge_covered_count > 0) {
                auto non_edge_probability = non_edge_sum / boost::rational<unsigned int>{ non_edge_covered_count };
                for (auto node : covered) {
                    if (!edge_covered.contains(node)) {
                        node->set_mine_probability(non_edge_probability);
                    }
                }
            }
        }


        MinesweeperSolver(Minesweeper game)
            : game { game } {
                auto board = game.get_board();
                boost::rational<unsigned int> default_probability { game.mines_left(), game.covered_tiles_count()};
                for (auto x = 0; x < game.width; x++) {
                    std::vector<Node> col;
                    for (auto y = 0; y < game.height; y++) {
                        auto n = Node(x, y);
                        n.set_value(board[x][y]);
                        col.push_back(n);
                    }
                    this->state.push_back(col);
                }

                for (auto x = 0; x < game.width; x++) {
                    bool link_left = x > 0;
                    for (auto y = 0; y < game.height; y++) {
                        bool link_right = x < game.width-1;
                        bool link_down = y < game.height-1;
                        if (link_right) {
                            this->state[x][y].add_adjacent(&this->state[x+1][y]);
                        }
                        if (link_down) {
                            this->state[x][y].add_adjacent(&this->state[x][y+1]);
                        }
                        if (link_right && link_down) {
                            this->state[x][y].add_adjacent(&this->state[x+1][y+1]);
                        }
                        if (link_left && link_down) {
                            this->state[x][y].add_adjacent(&this->state[x-1][y+1]);
                        }
                    }
                }
            }
        
        void update_state(Node* node) {
            auto [x, y] = node->coord();
            auto tile = game.get_tile(x, y);

            node->set_value(tile);
            if (tile == 0) {
                for (auto adjacent_tile : node->adjacent()) {
                    if (adjacent_tile->value() == Minesweeper::COVERED) {
                        update_state(adjacent_tile);
                    }
                }
            }
        }

        Minesweeper::GameState take_turn(Node* node) {
            last_clicked = node->coord();
            view_state();
            auto [x, y] = node->coord();
            auto game_state = game.uncover_tile(x, y);
            update_state(node);
            view_state();

            return game_state;
        }

        std::set<Node*> get_edge_nums() {
            std::set<Node*> edge_set;
            for (auto x = 0; x < this->game.width; x++) {
                for (auto y = 0; y < this->game.height; y++) {
                    if (this->state[x][y].edge_num()) {
                        edge_set.insert(&this->state[x][y]);
                    }
                }
            }

            return edge_set;
        }

        std::set<Node*> get_edge_covered() {
            std::set<Node*> edge_set;
            for (auto x = 0; x < this->game.width; x++) {
                for (auto y = 0; y < this->game.height; y++) {
                    if (this->state[x][y].edge_covered()) {
                        edge_set.insert(&this->state[x][y]);
                    }
                }
            }

            return edge_set;
        }

        void flag(Node* node) {
            last_clicked = node->coord();
            view_state();
            auto [x, y] = node->coord();
            game.toggle_flag(x, y);
            node->set_value(Minesweeper::FLAG);
            view_state();
        }

        void basic_flag(std::set<Node*> edge_nums) {
            for (auto node : edge_nums) {
                auto adjacent_covered = node->adjacent_covered_count();
                if (adjacent_covered == node->adjacent_mines_left()) {
                    for (auto adj_node: node->adjacent()) {
                        if (adj_node->value() == Minesweeper::COVERED) {
                            flag(adj_node);
                        }
                    }
                }
            }
        }

        std::vector<Node*> basic_safe(std::set<Node*> edge_covered) {
            std::vector<Node*> safe_tiles{};
            for (auto node : edge_covered) {
                if (node->safe()) {
                    safe_tiles.push_back(node);
                }
            }

            return safe_tiles;
        }

        std::vector<std::pair<Node*, bool>> compare_sets(std::set<Node*> set1, int set1_mines, std::set<Node*> set2, int set2_mines) {
            std::vector<std::pair<Node*, bool>> definitive;

            std::set<Node*> left, overlap, right;
            std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(overlap, overlap.begin()));
            auto overlap_count = overlap.size();

            if (overlap_count > 0) {
                std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(left, left.begin()));
                auto left_count = left.size();

                std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(right, right.begin()));
                auto right_count = right.size();

                if (left_count > 8 || right_count > 8) {
                    return definitive;
                }

                auto left_choose = set1_mines;
                auto overlap_choose = 0;
                auto right_choose = set2_mines;

                unsigned int left_picked, overlap_picked, right_picked, running_total;
                left_picked = overlap_picked = right_picked = running_total = 0;

                auto choose = choose_matrix<8,8>();
                while (left_choose >= 0 && right_choose >= 0) {
                    auto left_options = choose[left_count][left_choose];
                    auto overlap_options = choose[overlap_count][overlap_choose];
                    auto right_options = choose[right_count][right_choose];
                    auto total = left_options * overlap_options * right_options;

                    left_picked += left_choose * total;
                    overlap_picked += overlap_choose * total;
                    right_picked += right_choose * total;

                    running_total += total;
                    left_choose--;
                    overlap_choose++;
                    right_choose--;
                }

                if (left_picked == 0) {
                    for (auto node : left) {
                        definitive.push_back(std::pair{node, false});
                    }
                } else if (left_count > 0) {
                    for (auto node : left) {
                        boost::rational<unsigned int> probability{ left_picked / left_count , running_total };
                        if (probability.denominator() < node->mine_probability().denominator()) {
                            node->set_mine_probability(probability);
                        }
                        if (probability == 1) {
                            definitive.push_back(std::pair{node, true});
                        }
                    }
                }

                if (overlap_picked == 0) {
                    for (auto node : overlap) {
                        definitive.push_back(std::pair{node, false});
                    }
                } else if (overlap_count > 0) {
                    for (auto node : overlap) {
                        boost::rational<unsigned int> probability{ overlap_picked / overlap_count , running_total };
                        if (probability.denominator() < node->mine_probability().denominator()) {
                            node->set_mine_probability(probability);
                        }
                        if (probability == 1) {
                            definitive.push_back(std::pair{node, true});
                        }
                    }
                }

                if (right_picked == 0) {
                    for (auto node : right) {
                        definitive.push_back(std::pair{node, false});
                    }
                } else if (right_count > 0) {
                    for (auto node : right) {
                        boost::rational<unsigned int> probability{ right_picked / right_count , running_total };
                        if (probability.denominator() < node->mine_probability().denominator()) {
                            node->set_mine_probability(probability);
                        }
                        if (probability == 1) {
                            definitive.push_back(std::pair{node, true});
                        }
                    }
                }
            }
            
            return definitive;
        }

        bool try_advanced_solve(std::set<Node*> set1, int set1_mines, std::set<Node*> set2, int set2_mines) {
            auto definitive = compare_sets(set1, set1_mines, set2, set2_mines);
            if (definitive.size() > 0) {
                for (auto item : definitive) {
                    auto [node, mine] = item;
                    if (mine) {
                        flag(node);
                    } else {
                        take_turn(node);
                    }
                }
                return true;
            }
            return false;
        }

        std::set<Node*> all_covered() {
            std::set<Node*> covered;
            for (auto x = 0; x < game.width; x++) {
                for (auto y = 0; y < game.height; y++) {
                    if (state[x][y].value() == Minesweeper::COVERED) {
                        covered.insert(&state[x][y]);
                    }
                }
            }

            return covered;
        }

        bool advanced_solve(std::set<Node*> edge_nums) {
            for (auto node1 : edge_nums) {
                for (auto node2 : edge_nums) {
                    if (node1 == node2) {
                        continue;
                    }

                    if (try_advanced_solve(node1->adjacent_covered(), node1->adjacent_mines_left(), node2->adjacent_covered(), node2->adjacent_mines_left())) {
                        return true;
                    }
                }
                if (try_advanced_solve(node1->adjacent_covered(), node1->adjacent_mines_left(), all_covered(), game.mines_left())) {
                    return true;
                }
            }
            return false;
        }

        Minesweeper::GameState pick_least_probable() {
            // print_probability();
            calculate_probability();
            // print_probability();

            auto covered = all_covered();
            Node* least_probable = *covered.begin();
            for (auto node : covered) {
                if (node->mine_probability() < least_probable->mine_probability()) {
                    least_probable = node;
                }
            }
            return take_turn(least_probable);
        }

        void check_game_state(Minesweeper::GameState game_state) {
            if (game_state == Minesweeper::GameState::Lose) {
                throw std::logic_error("Oops! Clicked on a mine");
            } else if (game_state == Minesweeper::GameState::Win) {
                for (auto node : all_covered()) {
                    flag(node);
                }
                printf("Minefield Swept!\n");
                exit(0);
            }
        }

        const char* mode = "None";

        void solve() {
            view_state();

            auto x = this->game.width/2;
            auto y = this->game.height/2;
            Minesweeper::GameState game_state;
            game_state = take_turn(&state[x][y]);
            check_game_state(game_state);

            while (true) {
                mode = "Basic";
                auto edge_nums = get_edge_nums();
                basic_flag(edge_nums);

                auto edge_covered = get_edge_covered();
                auto safe_tiles = basic_safe(edge_covered);
                for (auto tile : safe_tiles) {
                    if (tile->value() == Minesweeper::COVERED) {
                        game_state = take_turn(tile);
                    }
                }
                check_game_state(game_state);

                if (safe_tiles.size() == 0) {
                    mode = "Advanced";
                    if (!advanced_solve(edge_nums)) {
                        mode = "Most probable (this might blow up)";
                        game_state = pick_least_probable();
                        check_game_state(game_state);
                    }
                }
            }
        }

        void view_state() {
            system("clear");
            printf("Solve Algorithm in Use: %s\n\n", mode);
            for (auto y = 0; y < this->game.height; y++) {
                for (auto x = 0; x < this->game.width; x++) {
                    if (x == last_clicked.first && y == last_clicked.second) {
                        printf("\x1b[32m");
                    }

                    auto value = this->state[x][y].value();
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

                    if (x == last_clicked.first && y == last_clicked.second) {
                        printf("\x1b[0m");
                    }
                }
                printf("\n");
            }
            printf("\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        void print_probability() {
            for (auto y = 0; y < this->game.height; y++) {
                for (auto x = 0; x < this->game.width; x++) {
                    printf("%5.2f ", boost::rational_cast<double>(this->state[x][y].mine_probability()));
                }
                printf("\n");
            }
            printf("\n");
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    };
}

// int main() {
//     // Get height, width, num mines (or give options: Beginner, intermediate, expert)

//     minesweeper::Minesweeper game(30, 16, 99);
//     minesweeper::solver::MinesweeperSolver solver(game);
//     solver.solve();
// }