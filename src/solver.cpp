#include <minesweeper.hpp>
#include <array>

using minesweeper::Minesweeper;

template<std::size_t N, std::size_t M>
    requires (N >= M)
constexpr std::array<std::array<unsigned int, M+1>, N+1> precomputed_choose() {
    std::array<std::array<unsigned int, M+1>, N+1> choose_matrix{};
    std::array<unsigned int, N+1> factorials{};

    factorials[0] = 1;
    for (auto i = 1; i < factorials.size(); i++) {
        factorials[i] = factorials[i-1] * i;
    }


    for (std::size_t n = 0; n <= N; n++) {
        auto k_max = std::min(M, n);
        for (auto k = 0; k <= k_max; k++) {
            choose_matrix[n][k] = factorials[n] / (factorials[n-k] * factorials[k]);
        }
    }

    return choose_matrix;
}

class MinesweeperSolver {
    Minesweeper game;
    std::vector<std::vector<double>> probability;

    void calculate_probability() {
        auto choose_matrix = precomputed_choose<8,8>();
        for (auto i = 0; i <= 8; i++) {
            for (auto j = 0; j <= 8; j++) {
                printf("%4d", choose_matrix[i][j]);
            }
            printf("\n");
        }
        double base_prob = this->game.mines_left() / this->game.covered_tiles_count();

        for (auto x = 0; x < this->game.width; x++) {
            for (auto y = 0; y < this->game.height; y++) {
                this->probability[x][y] = base_prob;
            }
        }
    }

    public:
    MinesweeperSolver(Minesweeper game)
        : game { game } {
            this->probability = std::vector(game.width, std::vector(game.height, 0.0));
        }
    
    void solve() {
        // calculate mine probability for each tile
        calculate_probability();
        // flag tiles w/ 100% probability
        // select tile w/ lowest probability
        // repeat
    }

    void view_state() {
        for (auto y = 0; y < this->game.height; y++) {
            for (auto x = 0; x < this->game.width; x++) {
                auto value = this->game.get_tile(x, y);
                switch (value) {
                    case Minesweeper::MINE:
                        printf("* ");
                        break;
                    case Minesweeper::FLAG:
                        printf("f ");
                        break;
                    case Minesweeper::COVERED:
                        printf("O ");
                        break;
                    case 0:
                        printf("  ");
                    default:
                        printf("%d ", value);
                }
            }
            printf("\n");
        }
    }
};

int main() {
    // Get height, width, num mines (or give options: Beginner, intermediate, expert)

    // Create & Run solver
    // Print state as we go
    Minesweeper game(10, 10, 10);
    MinesweeperSolver solver(game);
    solver.solve();
}