#include <minesweeper.hpp>
#include <boost/rational.hpp>
#include <set>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    class Node {
        std::pair<unsigned int, unsigned int> _coord;
        unsigned int _value = Minesweeper::COVERED;
        std::set<Node*> _adjacent{};
        boost::rational<unsigned int> _mine_probability{};
        unsigned int _adjacent_mines_left = 0;

    public:
        Node(unsigned int x, unsigned int y);

        std::pair<unsigned int, unsigned int> coord();

        unsigned int value();

        void set_value(unsigned int value);

        const std::set<Node*>& adjacent();

        std::set<Node*> adjacent_covered();

        unsigned int adjacent_covered_count();

        void add_adjacent(Node* node);

        boost::rational<unsigned int> mine_probability();

        void set_mine_probability(boost::rational<unsigned int> mp);

        unsigned int adjacent_mines_left();

        bool covered_edge();

        bool number_edge();

        bool covered_safe();
    };
}