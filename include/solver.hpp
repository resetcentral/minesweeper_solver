#include <minesweeper.hpp>
#include <boost/rational.hpp>
#include <set>

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
        std::array<std::array<unsigned int, M+1>, N+1> choose{};
        auto factorials = factorial_list<N>();

        for (std::size_t n = 0; n <= N; n++) {
            auto k_max = std::min(M, n);
            for (auto k = 0; k <= k_max; k++) {
                choose[n][k] = factorials[n] / (factorials[n-k] * factorials[k]);
            }
        }

        return choose;
    }

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

        bool safe();
    };
}