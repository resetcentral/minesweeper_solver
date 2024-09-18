#include <solver/node.hpp>
#include <stdexcept>


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

    bool Node::covered_edge() {
        if (_value == Minesweeper::COVERED) {
            for (auto node : _adjacent) {
                if (node->value() <= 8) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::number_edge() {
        if (_value <= 8) {
            for (auto node : _adjacent) {
                if (node->value() == Minesweeper::COVERED) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::covered_safe() {
        if (_value != Minesweeper::COVERED) {
            return false;
        }

        for (auto adj_node: _adjacent) {
            if (adj_node->value() <= 8 && adj_node->adjacent_mines_left() == 0) {
                return true;
            }
        }
        return false;
    }
}