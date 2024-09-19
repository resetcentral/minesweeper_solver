#include <minesweeper.hpp>
#include <boost/rational.hpp>
#include <set>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    class Node {
        std::pair<unsigned int, unsigned int> _coord;
        unsigned short _value = Minesweeper::COVERED;
        std::set<Node*> _adjacent{};
        boost::rational<unsigned int> _mine_probability{};
        unsigned short _adjacent_mines_left = 0;

    public:
        Node(unsigned int x, unsigned int y);

        const std::pair<unsigned int, unsigned int>& coord() const;

        unsigned short value() const;

        void set_value(const unsigned int short);

        const std::set<Node*>& adjacent() const;

        std::set<Node*> adjacent_covered() const;

        unsigned int adjacent_covered_count() const;

        void add_adjacent(Node* node);

        boost::rational<unsigned int> mine_probability() const;

        void set_mine_probability(boost::rational<unsigned int> mp);

        unsigned short adjacent_mines_left() const;

        bool covered_edge() const;

        bool number_edge() const;

        bool covered_safe() const;
    };
}