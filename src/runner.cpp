#include <solver/solver.hpp>
#include <iostream>
#include <tuple>

struct GameParameters {
    unsigned int width, height, mines;
};

int get_number_input(const char* prompt, int min) {
    int value;

    while (true) {
        std::cout << prompt << ": ";
        std::cin >> value;

        if (std::cin.fail() || value < min) {
            std::cout << "Invalid input. Cannot be less than " << min << "." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            return value;
        }
    }
}

GameParameters custom_input() {
    GameParameters params;

    params.width = get_number_input("Width", 1);
    params.height = get_number_input("Height", 1);
    params.mines = get_number_input("Number of mines", 0);

    return params;
}

int main() {
    // Get height, width, num mines (or give options: Beginner, intermediate, expert)
    std::cout << "Welcome to the Minesweeper solver!\n";
    std::cout << "[1] Beginner (9x9 - 10 mines)\n";
    std::cout << "[2] Intermediate (16x16 - 40 mines)\n";
    std::cout << "[3] Expert (30x16 - 99 mines)\n";
    std::cout << "[4] Custom\n";
    std::cout << "\nSelect an option [1-4]: ";

    int choice;
    std::cin >> choice;

    if (choice > 5 || choice < 1) {
        std::cout << "Invalid choice. Pick from 1 to 4" << std::endl;
    }

    GameParameters standard_games[3] = {
        GameParameters(9, 9, 10),   // Beginnner
        GameParameters(16, 16, 40), // Intermediate
        GameParameters(30, 16, 99)  // Expert
    };

    GameParameters chosen;
    switch (choice) {
        case 1:
        case 2:
        case 3: {   
            chosen = standard_games[choice-1];
            break;
        }
        case 4: {
            chosen = custom_input();
            break;
        }
        default: {
            std::cout << "Invalid choice" << std::endl;
            exit(1);
        }

    }

    auto animation_delay = get_number_input("Animation delay (ms)", 0);

    minesweeper::Minesweeper game(chosen.width, chosen.height, chosen.mines);
    minesweeper::solver::StateLogger logger(animation_delay);
    minesweeper::solver::MinesweeperSolver solver(game, logger);
    solver.solve();
}