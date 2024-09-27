#include <minesweeper.hpp>
#include <boost/rational.hpp>
#include <set>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;
    using Fraction = boost::rational<int>;

    /**
     * A node representing the Solver's knowledge of an associated tile
     * in a Minesweeper game.
     */
    class Node {
        const std::pair<unsigned int, unsigned int> _coord;
        unsigned short _value = Minesweeper::COVERED;
        std::set<Node*> _adjacent{};
        Fraction _mine_probability{};
        unsigned short _adjacent_mines_left = 0;

    public:
        /**
         * Create a node with the given (x,y) coordinates.
         * 
         * @param x x-coordinate of the associated Minesweeper tile
         * @param y y-coordinate of the associated Minesweeper tile
         */
        Node(unsigned int x, unsigned int y);

        // Node cannot be copied
        Node(const Node& other) = delete;
        Node& operator=(const Node& other) = delete;

        /**
         * Get the (x,y) coordinates of this Node.
         * 
         * @returns pair of (x,y) coordinates
         */
        std::pair<unsigned int, unsigned int> coord() const;

        /**
         * Get the value of this Node.
         * The value is one of:
         *  - An integer in [0-8]
         *  - Minesweeper::FLAG
         *  - Minesweeper::COVERED
         * 
         * @returns the value of the node
         */
        unsigned short value() const;

        /**
         * Set the value of this Node to the given value.
         * 
         * @param value the value to set
         */
        void set_value(const unsigned short value);
        

        /**
         * Get the set of nodes which are adjacent to this one.
         * 
         * @return set of pointers to adjacent Nodes
         */
        const std::set<Node*>& adjacent() const;

        /**
         * Add the given node to this Node's set of adjacent nodes, and adds
         * this Node to the given Node's set of adjacent nodes.
         * 
         * @param node the Node to add
         */
        void add_adjacent(Node* node);

        /**
         * Get the set of nodes which are adjacent to this one and also covered.
         * 
         * @return set of pointers to adjacent, covered Nodes
         */
        std::set<Node*> adjacent_covered() const;

        /**
         * Get the number of adjacent nodes that are covered.
         * 
         * @return number of adjacent covered Nodes
         */
        unsigned int adjacent_covered_count() const;

        /**
         * Get the set of nodes which are adjacent to this one that are
         * also hint tiles with at least one unflagged adjacent mine.
         * 
         * @return set of pointers to adjacent, active hint Nodes
         */
        std::set<Node*> adjacent_active_hints();

        /**
         * Get the probability of this Node being a mine.
         * 
         * @return rational number in [0, 1] representing a probability
         */
        Fraction mine_probability() const;

        /**
         * Set the probability of this Node being a mine.
         * 
         * @param mp a rational number in [0, 1] representing a probability
         * 
         * @throws std::invalid_argument if `mp` is outside of the range [0, 1]
         */
        void set_mine_probability(Fraction mp);

        /**
         * Get the number of mines adjacent to this node that haven't been
         * flagged yet.
         * 
         * This value is only valid when this node is representing an uncovered
         * hint tile.
         * 
         * @return number of adjacent unflagged mines
         */
        unsigned short adjacent_mines_left() const;

        /**
         * Is this node representing a hint tile?
         * 
         * @return is a hint tile
         */
        bool is_hint() const;

        /**
         * Is this node covered and adjacent to an uncovered node?
         * 
         * @return is covered and adjacent to an uncovered node
         */
        bool covered_edge() const;

        /**
         * Is this node a hint and adjacent to a covered node?
         * 
         * @return is a hint and adjacent to a covered node
         */
        bool hint_edge() const;

        /**
         * Is this node covered and adjacent to a hint node with no adjacent
         * mines left?
         * 
         * @return is covered and adjacent to a hint with no adjacent mines
         */
        bool covered_safe() const;

        ~Node();
    };
}