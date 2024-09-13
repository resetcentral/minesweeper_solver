#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <minesweeper.hpp>

namespace minesweeper {
    bool Minesweeper::out_of_bounds(const unsigned int x, const unsigned int y) {
        return x >= this->width || y >= this->height;
    }

    Minesweeper::Minesweeper(const unsigned int width, const unsigned int height, const unsigned int total_mines)
        : total_mines{ total_mines },
        width { width },
        height { height },
        covered_tiles { width * height } {
            if (covered_tiles == 0) {
                throw std::domain_error("Invalid dimensions");
            }

            if (total_mines >= covered_tiles) {
                throw std::invalid_argument("Too many mines");
            }

            MinefieldGenerator generator;
            this->underground = generator.generate(width, height, total_mines);
            this->visible = std::vector(width, std::vector(height, COVERED));
    }

    // move + copy semantics
    // allow array-like operations?

    unsigned short Minesweeper::get_tile(const unsigned int x, const unsigned int y) {
        if (out_of_bounds(x, y)) {
            throw std::out_of_range("Tile out of bounds");
        }

        return this->visible[x][y];
    }

    unsigned int Minesweeper::covered_tiles_count() noexcept {
        return this->covered_tiles;
    }

    unsigned int Minesweeper::flags_placed_count() noexcept {
        return this->flags_placed;
    }

    void Minesweeper::uncover_tile(const unsigned int x, const unsigned int y) {
        if (out_of_bounds(x, y)) {
            throw std::out_of_range("Tile out of bounds");
        }

        this->visible[x][y] = this->underground[x][y];
        this->covered_tiles--;

        const auto tile = this->visible[x][y];
        if (tile == MINE) {
            // game over
        }

        if (tile == 0) {
            // Uncover all tiles adjacent to this one
            for (auto i = x-1; i <= x+1; i++) {
                for (auto j = y-1; j <= y+1; j++) {
                    if (!out_of_bounds(i, j) && (i != x || j != y)) {
                        uncover_tile(i, j);
                    }
                }
            }
        }

        // check win state
        if (this->covered_tiles == this->total_mines) {
            // win
        }
        
    }

    void Minesweeper::toggle_flag(const unsigned int x, const unsigned int y) {
        if (out_of_bounds(x, y)) {
            throw std::out_of_range("Tile out of bounds");
        }

        auto tile = this->visible[x][y];
        switch (tile) {
            case COVERED: {
                this->visible[x][y] = FLAG;
                this->flags_placed++;
                break;
            }
            case FLAG: {
                this->visible[x][y] = COVERED;
                this->flags_placed--;
                break;
            }
            // Ignore if any other value
        }
    }

    void MinefieldGenerator::place_mines(Minefield* grid, const unsigned int width, const unsigned int height, const unsigned int total_mines) {
        std::uniform_int_distribution<unsigned int> x_dist(0, width-1);
        std::uniform_int_distribution<unsigned int> y_dist(0, height-1);

        auto mines_placed = 0;

        while (mines_placed != total_mines) {
            auto x = x_dist(this->rng);
            auto y = y_dist(this->rng);

            if ((*grid)[x][y] != Minesweeper::MINE) {
                (*grid)[x][y] = Minesweeper::MINE;
                mines_placed++;
            }
        }
    }

    void MinefieldGenerator::fill_hints(Minefield* grid, const unsigned int width, const unsigned int height) {
        for (auto x = 0; x < width; x++) {
            for (auto y = 0; y < height; y++) {
                if ((*grid)[x][y] == Minesweeper::MINE) {
                    for (auto i = x-1; i <= x+1; i++) {
                        for (auto j = y-1; j <= y+1; j++) {
                            if (i < width && j < height && (i != x || j != y) && (*grid)[i][j] != Minesweeper::MINE) {
                                (*grid)[i][j]++;
                            }
                        }
                    }
                }
            }
        }
    }

    MinefieldGenerator::MinefieldGenerator() {
        auto seed = std::random_device {}();
        this->rng = std::mt19937_64{ seed };
    }

    MinefieldGenerator::MinefieldGenerator(const unsigned int seed) {
        this->rng = std::mt19937_64{ seed };
    }
    
    Minefield MinefieldGenerator::generate(const unsigned int width, const unsigned int height, const unsigned int total_mines) {
        Minefield grid = std::vector(width, std::vector(height, static_cast<unsigned short>(0)));
        place_mines(&grid, width, height, total_mines);
        fill_hints(&grid, width, height);
        
        return grid;
    }
}