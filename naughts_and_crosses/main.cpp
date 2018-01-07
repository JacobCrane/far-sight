#include <iostream>

#include <naughts_and_crosses.hpp>

int main(int argc, char** argv)
{
    // Game instance
    NaughtsAndCrosses game;

    // Keep playing until game is done
    while (true)
    {
        // Player moves first (naughts), print current position;
        game.print_position();

        // Generate moves and offer choice
        game.generate_moves();
        unsigned move = game.print_moves_and_select();

        // Make move selected by player
        game.make_move(move);
        if (game.evaluate() == -1)
        {
            // Naughts win, so break
            std::cout << "Won!" << std::endl;
            break;
        }

        // Generate moves for computer
        game.generate_moves();
        if (game.candidates().size() == 0)
        {
            // No moves left, we have a draw. Break
            std::cout << "Draw!" << std::endl;
            break;
        }

        // Automatically choose computer's move and make it
        game.make_move(game.select_move());
        if (game.evaluate() == 1)
        {
            // Crosses win, so break
            std::cout << "Lost!" << std::endl;
            break;
        }
    }

    // Game over, print final position
    std::cout << "Final position:" << std::endl;
    game.print_position();

    return 0;
}

