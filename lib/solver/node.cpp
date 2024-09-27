#include <solver/node.hpp>
#include <stdexcept>


namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    Node::Node(unsigned int x, unsigned int y)
        : _coord { x, y } {}

    std::pair<unsigned int, unsigned int> Node::coord() const {
        return _coord;
    }

    unsigned short Node::value() const {
        return _value;
    }

    void Node::set_value(const unsigned short value) {
        _value = value;

        if (value == Minesweeper::FLAG) {
            set_mine_probability(1);
            for (auto node : _adjacent) {
                if (node->is_hint()) {
                    node->_adjacent_mines_left--;
                }
            }
        } else if (is_hint()) {
            set_mine_probability(0);
            _adjacent_mines_left = value;
            for (auto node : _adjacent) {
                if (node->value() == Minesweeper::FLAG) {
                    _adjacent_mines_left--;
                }
            }
        }
    }

    const std::set<Node*>& Node::adjacent() const {
        return _adjacent;
    }

    void Node::add_adjacent(Node* node) {
        if(_adjacent.insert(node).second) {
            node->add_adjacent(this);
        }
    }

    std::set<Node*> Node::adjacent_covered() const {
        std::set<Node*> covered;
        for (auto node : _adjacent) {
            if (node->value() == Minesweeper::COVERED) {
                covered.insert(node);
            }
        }
        return covered;
    }

    unsigned int Node::adjacent_covered_count() const {
        unsigned int count = 0;
        for (auto node : _adjacent) {
            if (node->value() == Minesweeper::COVERED) {
                count++;
            }
        }
        return count;
    }

    std::set<Node*> Node::adjacent_active_hints() {
        std::set<Node*> numbers;
        for (auto node : _adjacent) {
            if (node->is_hint() && node->adjacent_mines_left() > 0) {
                numbers.insert(node);
            }
        }
        return numbers;
    }

    Fraction Node::mine_probability() const {
        return _mine_probability;
    }

    void Node::set_mine_probability(Fraction mp) {
        if (mp < 0 || mp > 1) {
            throw std::invalid_argument("Mine probability cannot be less than 0 or greater than 1.");
        }

        _mine_probability = mp;
    }

    unsigned short Node::adjacent_mines_left() const {
        return _adjacent_mines_left;
    }

    bool Node::is_hint() const {
        return _value >= 0 && _value <= 8;
    }

    bool Node::covered_edge() const {
        if (_value == Minesweeper::COVERED) {
            for (auto node : _adjacent) {
                if (node->is_hint()) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::hint_edge() const {
        if (is_hint()) {
            for (auto node : _adjacent) {
                if (node->value() == Minesweeper::COVERED) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::covered_safe() const {
        if (_value != Minesweeper::COVERED) {
            return false;
        }

        for (auto adj_node: _adjacent) {
            if (adj_node->is_hint() && adj_node->adjacent_mines_left() == 0) {
                return true;
            }
        }
        return false;
    }

    Node::~Node() {
        printf("Destroying node: %d %d\n", _coord.first, _coord.second);
    }
}