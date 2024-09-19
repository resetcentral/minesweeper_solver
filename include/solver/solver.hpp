#include <solver/choose.hpp>
#include <solver/node.hpp>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    class SolverState {
        std::vector<std::vector<Node>> state;
        Node* _selected;

    public:
        SolverState(const Minefield& field);

        unsigned int width();

        unsigned int height();

        std::set<Node*> covered();

        std::set<Node*> number_edge();

        std::set<Node*> covered_edge();

        void update(Node* node, const Minefield* minefield);

        const Node& selected();

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
        std::set<std::pair<Node*, bool>> compare_sets(std::set<Node*> set1, int set1_mines, std::set<Node*> set2, int set2_mines);

    public:
        std::set<std::pair<Node*, bool>> solve(SolverState state, int mines_left);

    };

    class ProbableSolver {
        void calculate_probability(SolverState state, int mines_left);

    public:
        Node* solve(SolverState state, int mines_left);
    };

    class MinesweeperSolver {
        Minesweeper game;
        SolverState state;
        StateLogger logger;

        void flag_or_uncover(Node* node, bool flag);

        void check_game_state(Minesweeper::GameState game_state);

    public:
        MinesweeperSolver(Minesweeper game, StateLogger logger);

        void solve();
    };
}