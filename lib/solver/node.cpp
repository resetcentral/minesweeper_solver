#include <solver/node.hpp>
#include <stdexcept>


namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    Node::Node(unsigned int x, unsigned int y)
        : _coord { x, y } {}

    const std::pair<unsigned int, unsigned int>& Node::coord() const {
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

    const std::set<Node*>& Node::adjacent() const {
        return _adjacent;
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

    void Node::add_adjacent(Node* node) {
        if(_adjacent.insert(node).second) {
            node->add_adjacent(this);
        }
    }

    boost::rational<unsigned int> Node::mine_probability() const {
        return _mine_probability;
    }

    void Node::set_mine_probability(boost::rational<unsigned int> mp) {
        if (mp > 1) {
            throw std::invalid_argument("Mine probability cannot be less than 0 or greater than 1.");
        }

        _mine_probability = mp;
    }

    unsigned short Node::adjacent_mines_left() const {
        return _adjacent_mines_left;
    }

    bool Node::covered_edge() const {
        if (_value == Minesweeper::COVERED) {
            for (auto node : _adjacent) {
                if (node->value() <= 8) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Node::number_edge() const {
        if (_value <= 8) {
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
            if (adj_node->value() <= 8 && adj_node->adjacent_mines_left() == 0) {
                return true;
            }
        }
        return false;
    }
}