#include <minesweeper.hpp>
#include <boost/rational.hpp>
#include <set>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;
    using fraction = boost::rational<int>;

    class Node {
        std::pair<unsigned int, unsigned int> _coord;
        unsigned short _value = Minesweeper::COVERED;
        std::set<Node*> _adjacent{};
        fraction _mine_probability{};
        unsigned short _adjacent_mines_left = 0;

    public:
        Node(unsigned int x, unsigned int y);
        Node(const Node& other) = delete;
        Node& operator=(const Node& other) = delete;

        const std::pair<unsigned int, unsigned int>& coord() const;

        unsigned short value() const;

        void set_value(const unsigned int short);

        const std::set<Node*>& adjacent() const;

        std::set<Node*> adjacent_covered() const;

        std::set<Node*> adjacent_active_numbers();

        unsigned int adjacent_covered_count() const;

        void add_adjacent(Node* node);

        fraction mine_probability() const;

        void set_mine_probability(fraction mp);

        unsigned short adjacent_mines_left() const;

        bool covered_edge() const;

        bool number_edge() const;

        bool covered_safe() const;

        ~Node();
    };
}