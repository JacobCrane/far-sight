#ifndef GAME_HPP
#define GAME_HPP

#include <array>
#include <iostream>
#include <vector>

class NaughtsAndCrosses
{
    protected:
        // Renaming a 3x3 array of integers that represents our board. The
        // nine integers tell us which player (if any) occupies the nine
        // squares of the game board
        using Board_t = std::array<std::array<int, 3>, 3>;
        // Current game board
        Board_t m_board {};
        // History of positions (useful for making/undoing moves rapidly)
        std::vector<Board_t> m_history {};

        // "Bitboards" representing each player's pieces. Each of these
        // bitboards contains nine bits which will be used (uint16_t has a
        // width of 16 bits but we won't be using the more significant bits).
        // Each bit represents a single square on the board, if the bit is set
        // then that player has their marker on the corresponding spot on the
        // game board.
        //
        // Least significant bit (LSB) is the top-left square of the board, and
        // most significant bit (MSB) is the bottom right. We shall store values
        // as hexadecimal. This representation makes checking for wins easy.
        //
        // EXAMPLES:
        // Board:   - - O
        //          - - -       m_naughts = 0x4     (binary 000000100)
        //          - - -       m_crosses = 0x0     (binary 000000000)
        //
        // Board:   - - -
        //          - X O       m_naughts = 0x120   (binary 100100000)
        //          - - O       m_crosses = 0x10    (binary 000010000)
        //
        // Board:   X O X
        //          - X O       m_naughts = 0xe2    (binary 011100010)
        //          O O -       m_crosses = 0x15    (binary 000010101)
        uint16_t m_naughts = 0;
        uint16_t m_crosses = 0;

        // Vector containing all locations currently unoccupied (0 represents
        // the LSB, i.e. the top-left corner, and 8 represents the MSB, i.e.
        // the bottom-right corner. The board (with index numbers) looks like
        //
        // 0 1 2
        // 3 4 5
        // 6 7 8
        //
        // In the examples given above, the
        // corresponding vector of candidates would be
        //
        // {0, 1, 3, 4, 5, 6, 7, 8}
        // {0, 1, 2, 3, 6, 7}
        // {3, 8}
        //
        // respectively
        std::vector<unsigned> m_candidates;

        // Values representing the two players, one is the negation of the other
        // for reasons related to the move-searching algorithm (see later).
        static const int naught = -1;
        static const int cross = 1;
        // Starting player is naughts (arbitrary choice)
        int m_player = naught;

    public:
        // Simply return read-only data: current player to move and the list of
        // currently available moves
        int current_player() const;
        const std::vector<unsigned>& candidates() const;

        // Generate bitboards from the array representation of the board
        void update_bitboards();
        // Find empty squares on the game board
        void generate_moves();
        // Evaluate the current position
        int evaluate() const;

        // Print current position to standard out
        void print_position() const;
        // List possible moves and get input (for player)
        unsigned print_moves_and_select() const;

        // Recursively evaluate positions for minimax. See
        //     https://chessprogramming.wikispaces.com/Negamax
        // and 
        //     https://en.wikipedia.org/wiki/Negamax
        int negamax(const unsigned& depth);
        // Evaluate possible moves and make choice automatically (for computer)
        unsigned select_move();
        // Make move on array board, then update bitboards
        void make_move(unsigned position);
        // Undo last move
        void undo_move();
};

#endif  // GAME_HPP

