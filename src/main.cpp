#include "Game.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "Game.h"
// ******************** MAIN FUNCTION ********************

int main() {
    try {
        // Create and run the game
        Game game;
        game.Run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

