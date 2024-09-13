#include <vector>
#include <random>

namespace minesweeper {
    using Minefield = std::vector<std::vector<unsigned short>>;

    class Minesweeper {
        Minefield visible;
        Minefield underground;
        unsigned int flags_placed {};
        unsigned int covered_tiles;

        bool out_of_bounds(const unsigned int x, const unsigned int y);

    public:
        inline static const unsigned short MINE = 9;
        inline static const unsigned short FLAG = 10;
        inline static const unsigned short COVERED = 11;
        const unsigned int width, height;
        const unsigned int total_mines;

        Minesweeper(const unsigned int width, const unsigned int height, const unsigned int total_mines);

        unsigned short get_tile(const unsigned int x, const unsigned int y);

        unsigned int covered_tiles_count() noexcept;

        unsigned int flags_placed_count() noexcept;

        void uncover_tile(const unsigned int x, const unsigned int y);

        void toggle_flag(const unsigned int x, const unsigned int y);
    };

    class MinefieldGenerator {
        std::mt19937_64 rng;

        void place_mines(Minefield* grid, const unsigned int width, const unsigned int height, const unsigned int total_mines);

        void fill_hints(Minefield* grid, const unsigned int width, const unsigned int height);

    public:
        MinefieldGenerator();

        MinefieldGenerator(const unsigned int seed);
        
        Minefield generate(const unsigned int width, const unsigned int height, const unsigned int total_mines);
    };
}