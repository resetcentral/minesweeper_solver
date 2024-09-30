#include <solver/sle.hpp>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    class SolverState {
        std::vector<std::vector<Node*>> state;
        Node* _selected;

    public:
        /**
         * Create a state representation for a Solver based on the known
         * values in the given Minesweeper Minefield.
         * 
         * @param minefield Minefield used to intialize the state with values
         */
        SolverState(const Minefield& minefield);

        /**
         * Get the width of the minefield represented by this state
         * 
         * @return width of the minefield
         */
        unsigned int width() const;

        /**
         * Get the height of the minefield represented by this state
         * 
         * @return height of the minefield
         */
        unsigned int height() const;

        /**
         * Get all nodes that are known to be covered.
         * 
         * @return set of all Nodes in the state with value Tile::Covered
         */
        std::set<Node*> covered();

        /**
         * Get all hint nodes that are adjacent to a covered node.
         * 
         * Hint nodes are Nodes with a value in [Tile::HintMin, Tile::HintMax].
         * 
         * @return set of all hint Nodes that are adjacent to a covered node
         */
        std::set<Node*> hint_edge();

        /**
         * Get all covered nodes that are adjacent to a hint node.
         * 
         * @return set of all covered nodes that are adjacent to a hint node
         */
        std::set<Node*> covered_edge();

        /**
         * Update the value of `node` based on its value in `minefield`.
         * 
         * If the value of `node` is 0, all adjacent nodes are also updated.
         * 
         * @param node pointer to the node to update
         * @param minefield known values of tiles in the Minesweeper Minefield
         */
        void update(Node* node, const Minefield& minefield);

        /**
         * Get the currently selected node.
         * 
         * The selected node is typically the node that was just updated or is
         * about to be updated.
         * 
         * @return selected node
         */
        const Node& selected() const;

        /**
         * Set the currently selected node.
         * 
         * @param node selected node
         */
        void set_selected(Node* node);

        /**
         * Get the node at the given (x,y) coordinates.
         * 
         * @param x x-coordinate of the node
         * @param y y-coordinate of the node
         * 
         * @return Node at (x,y) in the SolverState
         */
        Node* get_node(const unsigned int x, const unsigned int y);
    };

    class StateLogger {
        const char* _mode = "";
        unsigned int animation_delay;
    public:
        StateLogger(unsigned int animation_delay_ms);

        void set_mode(const char* mode);

        void log(SolverState state);
    };

    class BasicSolver {
    public:
        std::set<Node*> flaggable(SolverState state);

        std::set<Node*> safe(SolverState state);
    };

    class AdvancedSolver {
    public:
        std::set<Node*> flaggable(SolverState state);

        std::set<Node*> safe(SolverState state);
    };

    class ProbableSolver {
        void print_probabilities(SolverState state);

        static bool validate_assignments(sle::Assignments assignments);

        sle::Assignments bruteforce(sle::SystemOfLinearEquations& sys_eq, const std::set<Node*>& ind_vars);

    public:
        void calculate_probability(SolverState state, int mines_left);

        Node* solve(SolverState state, int mines_left);
    };

    class MinesweeperSolver {
        Minesweeper game;
        SolverState state;
        StateLogger logger;

        void flag_or_uncover(Node* node, bool flag);

        void flag_all(std::set<Node*> nodes);

        void uncover_all(std::set<Node*> nodes);

        void check_game_state(Minesweeper::GameState game_state);

    public:
        MinesweeperSolver(Minesweeper game, StateLogger logger);

        void solve();
    };
}