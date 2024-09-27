#include <minesweeper.hpp>
#include <algorithm>
#include <stdexcept>

namespace minesweeper {
    // MinefieldGenerator
    /**
     * Place `total_mines` mines on the Minefield `grid`.
     * 
     * @param grid Minefield to place mines on
     * @param width width of `grid`
     * @param height height of `grid`
     * @param total_mines number of mines to place
     */
    void MinefieldGenerator::place_mines(Minefield& grid, const unsigned int width, const unsigned int height, const unsigned int total_mines) {
        std::uniform_int_distribution<unsigned int> x_dist(0, width-1);
        std::uniform_int_distribution<unsigned int> y_dist(0, height-1);

        auto mines_placed = 0;

        while (mines_placed != total_mines) {
            auto x = x_dist(rng);
            auto y = y_dist(rng);

            if (grid[x][y] != Tile::Mine) {
                grid[x][y] = Tile::Mine;
                mines_placed++;
            }
        }
    }

    /**
     * Fill out the hint tiles in the Minefield `grid` based on mines that have
     * already been placed.
     * 
     * Assumes that the grid has been initialized with all zeroes except for the mines.
     * 
     * @param grid Minefield to fill out
     * @param width width of `grid`
     * @param height height of `grid`
     */
    void MinefieldGenerator::fill_hints(Minefield& grid, const unsigned int width, const unsigned int height) {
        for (auto x = 0; x < width; x++) {
            for (auto y = 0; y < height; y++) {
                if (grid[x][y] == Tile::Mine) {
                    // Add 1 to all tiles adjacent to this one
                    for (auto i = x-1; i <= x+1; i++) {
                        for (auto j = y-1; j <= y+1; j++) {
                            if (i < width && j < height && (i != x || j != y) && grid[i][j] != Tile::Mine) {
                                grid[i][j] = Tile(grid[i][j]+1);
                            }
                        }
                    }
                }
            }
        }
    }

    MinefieldGenerator::MinefieldGenerator() {
        auto rd = std::random_device {};
        auto seed = rd();
        rng = std::mt19937_64{ seed };
    }

    MinefieldGenerator::MinefieldGenerator(const unsigned int seed) {
        rng = std::mt19937_64{ seed };
    }
    
    Minefield MinefieldGenerator::generate(const unsigned int width, const unsigned int height, const unsigned int total_mines) {
        Minefield grid = std::vector(width, std::vector(height, Tile(0)));
        place_mines(grid, width, height, total_mines);
        fill_hints(grid, width, height);
        
        return grid;
    }


    // Minesweeper
    Minesweeper::Minesweeper(MinefieldGenerator generator, unsigned int width, const unsigned int height, const unsigned int total_mines)
        : total_mines{ total_mines },
        width { width },
        height { height },
        covered_tiles { width * height } {
            if (covered_tiles == 0) {
                throw std::invalid_argument("Invalid dimensions");
            }

            if (total_mines >= covered_tiles) {
                throw std::invalid_argument("Too many mines");
            }
            
            field = generator.generate(width, height, total_mines);
            visible = std::vector(width, std::vector(height, Tile::Covered));
    }

    Minesweeper::Minesweeper(unsigned int width, const unsigned int height, const unsigned int total_mines)
        : Minesweeper::Minesweeper(MinefieldGenerator {}, width, height, total_mines) {}
    
    /**
     * Are the given coordinates outside of the game field?
     * 
     * @param x x-coordinate to check
     * @param y y-coordinate to check
     * 
     * @return coordinates are outside the game field
     */
    bool Minesweeper::out_of_bounds(const int x, const int y) const noexcept {
        return x < 0 || x >= width || y < 0 || y >= height;
    }

    /**
     * Throw if the given coordinates are not inside the game field.
     * 
     * @param x x-coordinate to check
     * @param y y-coordinate to check
     * 
     * @throws std::out_of_range if (x,y) is not inside the game field
     */
    void Minesweeper::bounds_check(const int x, const int y) const {
        if (out_of_bounds(x, y)) {
            throw std::out_of_range("Tile out of bounds");   
        }
    }

    Tile Minesweeper::get_tile(const int x, const int y) const {
        bounds_check(x, y);

        return visible[x][y];
    }

    unsigned int Minesweeper::covered_tiles_count() const noexcept {
        return covered_tiles;
    }

    unsigned int Minesweeper::flags_placed_count() const noexcept {
        return flags_placed;
    }

    const Minefield& Minesweeper::get_field() const noexcept {
        return visible;
    }

    int Minesweeper::mines_left() const {
        return static_cast<int>(total_mines) - flags_placed;
    }

    Minesweeper::GameState Minesweeper::uncover_tile(const int x, const int y) {
        bounds_check(x, y);

        if (visible[x][y] != Tile::Covered) {
            return Minesweeper::GameState::Continue;
        }

        const auto tile = field[x][y];
        visible[x][y] = tile;
        covered_tiles--;

        if (tile == Tile::Mine) {
            return Minesweeper::GameState::Lose;
        }
        
        if (tile == 0) {
            // Uncover all tiles adjacent to this one
            for (auto i = x-1; i <= x+1; i++) {
                for (auto j = y-1; j <= y+1; j++) {
                    if (!out_of_bounds(i, j) && !(i == x && j == y)) {
                        uncover_tile(i, j);
                    }
                }
            }
        }

        if (covered_tiles == total_mines) {
            return Minesweeper::GameState::Win;
        } else {
            return Minesweeper::GameState::Continue;
        }
    }

    void Minesweeper::toggle_flag(const int x, const int y) {
        bounds_check(x, y);

        auto tile = visible[x][y];
        switch (tile) {
            case Tile::Covered: {
                visible[x][y] = Tile::Flag;
                flags_placed++;
            }
                break;
            case Tile::Flag: {
                visible[x][y] = Tile::Covered;
                flags_placed--;
            }
                break;
            default:
                break;
            // Ignore if any other value
        }
    }
}