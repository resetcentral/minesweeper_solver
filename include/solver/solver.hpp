#include <solver/sle.hpp>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    class SolverState {
        std::vector<std::vector<Node*>> state;
        Node* _selected;

    public:
        SolverState(const Minefield& minefield);

        unsigned int width() const;

        unsigned int height() const;

        std::set<Node*> covered();

        std::set<Node*> hint_edge();

        std::set<Node*> covered_edge();

        void update(Node* node, const Minefield& minefield);

        const Node& selected() const;

        void set_selected(Node* node);

        Node* get_node(const unsigned int x, const unsigned int y);
    };

    class StateLogger {
        const char* _mode = "";
    public:
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