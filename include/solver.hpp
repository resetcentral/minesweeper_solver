#include <minesweeper.hpp>

namespace minesweeper::solver {
    using minesweeper::Minesweeper;

    /**
     * Generate a list of factorials from 0! to `N`!
     * 
     * @param N std::size_t last number to generate a factorial for
     * @return array of factorials from 0! to `N`!
    */
    template<std::size_t N>
    constexpr std::array<unsigned int, N+1> factorial_list() {
        std::array<unsigned int, N+1> factorials{};

        factorials[0] = 1;
        for (auto i = 1; i < factorials.size(); i++) {
            factorials[i] = factorials[i-1] * i;
        }

        return factorials;
    }

    /**
     * Compute choose function from (0 choose 0) to (`N` choose `M`)
     * 
     * @param N size_t number of items in the set
     * @param M size_t number of items to choose
     * @return matrix of results of the choose function from (0 choose 0) to (`N` choose `M`)
     */
    template<std::size_t N, std::size_t M>
        requires (N >= M)
    constexpr std::array<std::array<unsigned int, M+1>, N+1> choose_matrix() {
        std::array<std::array<unsigned int, M+1>, N+1> choose_matrix{};
        auto factorials = factorial_list<N>();

        for (std::size_t n = 0; n <= N; n++) {
            auto k_max = std::min(M, n);
            for (auto k = 0; k <= k_max; k++) {
                choose_matrix[n][k] = factorials[n] / (factorials[n-k] * factorials[k]);
            }
        }

        return choose_matrix;
    }
}