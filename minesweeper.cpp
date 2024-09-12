#include <vector>
#include <random>
#include <chrono>

using MineGrid = std::vector<std::vector<unsigned short>>;

class MinesweeperGenerator {
    std::mt19937_64 rng;

    void place_mines(MineGrid grid, const unsigned int width, const unsigned int height, const unsigned int total_mines) {
        auto mines_placed = 0;

        while (mines_placed != total_mines) {
            auto x = this->rng() % width;
            auto y = this->rng() % height;

            if (grid[x][y] != Minesweeper::MINE) {
                grid[x][y] = Minesweeper::MINE;
                mines_placed++;
            }
        }
    }

    void fill_hints(MineGrid grid, const unsigned int width, const unsigned int height) {
        for (auto x = 0; x < width; x++) {
            for (auto y = 0; y < height; y++) {
                if (grid[x][y] == Minesweeper::MINE) {
                    // add 1 to adjacent (if not MINE)
                }
            }
        }
    }

public:
    MinesweeperGenerator() {
        auto seed = std::random_device {}();
        this->rng = std::mt19937_64{ seed };
    }

    MinesweeperGenerator(const unsigned int seed) {
        this->rng = std::mt19937_64{ seed };
    }
    
    MineGrid generate(const unsigned int width, const unsigned int height, const unsigned int total_mines) {
        MineGrid grid = std::vector(width, std::vector(height, static_cast<unsigned short>(0)));
        place_mines(grid, width, height, total_mines);
        fill_hints(grid, width, height);
        
        return grid;
    }
};

class Minesweeper {
    
    MineGrid visible;
    MineGrid underground;
    unsigned int flags_placed {};
    unsigned int covered_tiles;

    bool out_of_bounds(const unsigned int x, const unsigned int y) {
        return x > this->width || y > this->height;
    }

public:
    static const unsigned short MINE = 9;
    static const unsigned short FLAG = 10;
    static const unsigned short COVERED = 11;
    const unsigned int width, height;
    const unsigned int total_mines;

    Minesweeper(const unsigned int width, const unsigned int height, const unsigned int total_mines)
        : total_mines{ total_mines },
          width { width },
          height { height },
          covered_tiles { width * height } {
            if (covered_tiles == 0) {
                // throw invalid dimensions
            }

            if (total_mines >= covered_tiles) {
                // throw too many mines
            }

            MinesweeperGenerator generator;
            this->underground = generator.generate(width, height, total_mines);

            this->visible = std::vector(width, std::vector(height, COVERED));
    }

    ~Minesweeper() {

    }

    // move + copy semantics
    // allow array-like operations?

    unsigned short get_tile(const unsigned int x, const unsigned int y) {
        if (out_of_bounds(x, y)) {
            //throw
        }

        return this->visible[x][y];
    }

    unsigned int covered_tiles() noexcept {
        return this->covered_tiles;
    }

    unsigned int flags_placed() noexcept {
        return this->flags_placed;
    }

    void uncover_tile(const unsigned int x, const unsigned int y) {
        if (out_of_bounds(x, y)) {
            // throw
        }

        this->visible[x][y] = this->underground[x][y];
        this->covered_tiles--;

        const auto tile = this->visible[x][y];
        if (tile == MINE) {
            // game over
        }

        if (tile == 0) {
            // Uncover all tiles adjacent to this one
            for (auto i = x-1; i < x+1; i++) {
                for (auto j = y-1; j < y+1; j++) {
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

    void toggle_flag(const unsigned int x, const unsigned int y) {
        if (out_of_bounds(x, y)) {
            // throw
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
};