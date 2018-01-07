#include <naughts_and_crosses.hpp>

// Return current player
int NaughtsAndCrosses::current_player() const
{
    return m_player;
}

// Return list of possible moves
const std::vector<unsigned>& NaughtsAndCrosses::candidates() const
{
    return m_candidates;
}


// Generate bitboards from the array representation of the board
void NaughtsAndCrosses::update_bitboards()
{
    // Reset bitboards
    m_naughts = m_crosses = 0;
    for (unsigned i(0); i < 9; ++i)
    {
        // Visit array board squares in order m_board[0][0], m_board[0][1],
        // m_board[0][2], m_board[1][0], ..., m_board[2][1], m_board[2][2]
        //
        // If the current square contains a naught, set the correct bit in the
        // naughts bitboard
        if (m_board[i/3][i % 3] == naught)
            m_naughts |= (1 << i);
        // If the current square contains a cross, set the correct bit in the
        // crosses bitboard
        else if (m_board[i/3][i % 3] == cross)
            m_crosses |= (1 << i);
    }
}

// Find empty squares on the game board
void NaughtsAndCrosses::generate_moves()
{
    // Clear candidates list
    m_candidates.clear();

    // Mark all squares as available
    uint16_t available = 0x1ff;
    // Clear bits representing squares occupid by naughts or crosses
    available ^= (m_naughts | m_crosses);

    // Loop through bits of the available mask
    for (unsigned pos {0}; pos < 9; ++pos)
    {
        // If nth bit is set, add n to the list of available moves
        if (available & (1 << pos))
            m_candidates.push_back(pos);
    }
}

// Evaluate the current position
int NaughtsAndCrosses::evaluate() const
{
    // Check naughts and crosses bitboards for a comleted column, row, or
    // diagonal, and return -1 for a naughts win or +1 for a crosses win. If
    // neither player has a win, return 0.
    //
    // If we have a two sets of bits X and Y, then X & Y leaves only the set
    // bits in common between the two. Checking if (X & Y) == Y in effect
    // checks if X contains the whole of Y, i.e. if we check the set of
    // naughts against every possible completed column, row, and diagonal, then
    // we can find out if a win has been registered
    //
    // Bitmask examples:
    // 0x7:     * * *
    //          - - -
    //          - - -
    // 
    // 0x124:   - - *
    //          - - *
    //          - - *
    //
    // 0x111:   * - -
    //          - * -
    //          - - *
    if (((m_naughts & 0x7) == 0x7) | ((m_naughts & 0x38) == 0x38)
            | ((m_naughts & 0x1c0) == 0x1c0) | ((m_naughts & 0x49) == 0x49)
            | ((m_naughts & 0x92) == 0x92) | ((m_naughts & 0x124) == 0x124)
            | ((m_naughts & 0x111) == 0x111) | ((m_naughts & 0x54) == 0x54))
        return naught;
    if (((m_crosses & 0x7) == 0x7) | ((m_crosses & 0x38) == 0x38)
            | ((m_crosses & 0x1c0) == 0x1c0) | ((m_crosses & 0x49) == 0x49)
            | ((m_crosses & 0x92) == 0x92) | ((m_crosses & 0x124) == 0x124)
            | ((m_crosses & 0x111) == 0x111) | ((m_crosses & 0x54) == 0x54))
        return cross;
    return 0;
}

// Print current position to standard out
void NaughtsAndCrosses::print_position() const
{
    // Loop through rows of array board
    for (unsigned i(0); i < 3; ++i)
    {
        // Loop through columns of array board
        for (unsigned j(0); j < 3; ++j)
            // Equivalent to:
            //
            // if (m_board[i][j] == naught)
            //     std::cout << "O ";
            // else if (m_board[i][j] == cross)
            //     std::cout << "X ";
            // else
            //     std::cout << "- ";
            std::cout << (m_board[i][j] == naught ? "O " :
                    (m_board[i][j] == cross ? "X " : "- "));
        // New row
        std::cout << std::endl;
    }
}

// List possible moves and get input (for player)
unsigned NaughtsAndCrosses::print_moves_and_select() const
{
    std::cout << "Available:" << std::endl;
    // To store the selected move
    unsigned selection;
    // Loop through candidate moves
    for (unsigned i(0); i < m_candidates.size(); ++i)
        // Print move number (index in list) and the coordinates of the square
        // pointed to
        std::cout << "[" << i << "]: (" << m_candidates[i]/3 << ", "
            << m_candidates[i] % 3 << ")" << std::endl;
    // Get input from player
    std::cout << "Choice: ";
    std::cin >> selection;
    // Invalid input number (not at all a complete validation, if you even look
    // at the prompt funny then this will crash hard. Just make sure you input 
    // a number from the list of available moves)
    while (selection >= m_candidates.size())
    {
        std::cout << "Selection invalid, choose again: ";
        std::cin >> selection;
    }

    // Return the selected move
    return m_candidates[selection];
}

// Recursively evaluate positions for minimax
int NaughtsAndCrosses::negamax(const unsigned& depth)
{
    // Store a player-independent score (-1 for a lost position for the player
    // to move, +1 for a won position)
    int position_score {current_player()*evaluate()};
    // Reached a final position, just return the position score
    if (depth == 0)
        return position_score;
    // This position is not a final position but is a win for one player or the
    // other. Return the score.
    if (position_score != 0)
        return position_score;

    // If we get to this point, this position hasn't been won by either player,
    // so we need to recurse. Generate possible moves in this position
    generate_moves();
    // Get copy of move list (since the list of candidate moves will be trashed
    // by any recursion that occurs
    std::vector<unsigned> move_list {m_candidates};

    // We're looking to maximise our position score, start with a minimum score
    // (-1 represents an opponent win) and search for anything better. score
    // contains the score of the currently-being-evaluated move
    int max(-1), score(0);

    // Loop through available moves
    for (unsigned i {0}; i < move_list.size(); ++i)
    {
        // Make the move
        make_move(move_list[i]);
        // From the note above for the position_score variable, score will
        // contain +1 for a winning move for the player to move, -1 for a losing
        // move for the player, and 0 for a move which is neither (a draw)
        score = -negamax(depth - 1);
        // Update maximum score of coming down this branch in the move tree
        if (max < score)
            max = score;
        // Return to last position
        undo_move();
    }

    // Return the maximum score we can expect coming down this branch in the
    // move tree
    return max;
}

// Evaluate possible moves and make choice automatically (for computer)
unsigned NaughtsAndCrosses::select_move()
{
    // Take a copy of the current candidates list (since this will be changed
    // during the recursion)
    std::vector<unsigned> move_list {m_candidates};
    // The depth of the search should be to the end of the game (since for
    // naughts and crosses this is easy to do), and this means the depth needs
    // to be the number of available moves
    std::size_t depth {move_list.size()};

    // We're looking to maximise our position score, start with a minimum score
    // (-1 represents an opponent win) and search for anything better. score
    // contains the score of the currently-being-evaluated move
    int max(-1), score(0);
    // Index of the maximally-scoring move in the move list
    unsigned max_index(0);

    // Loop through moves
    for (unsigned i {0}; i < move_list.size(); ++i)
    {
        // Make the move
        make_move(move_list[i]);
        // Begin recursion on this position (flipping the score because one
        // player's winning position is the other player's loss. See references
        // for negamax/minimax. Also reduce the depth by one for the recursion.
        score = -negamax(depth - 1);
        // Update maximum score if this move is better than what we have stored.
        if (max < score)
        {
            max = score;
            max_index = i;
        }
        // Return to last position
        undo_move();
    }

    // Re-populate move list that has been trashed by the recursion
    generate_moves();
    // Return a maximally-scoring move
    return move_list[max_index];
}

// Make move on array board, then update bitboards
void NaughtsAndCrosses::make_move(unsigned position)
{
    // Save position in historical record
    m_history.push_back(m_board);
    // Set chosen position to be the curren player's
    m_board[position/3][position % 3] = m_player;
    // Update bitboards
    update_bitboards();
    // Flip current player
    m_player *= -1;
}

// Undo last move
void NaughtsAndCrosses::undo_move()
{
    // Restore position from the historical record
    m_board = m_history.back();
    // Remove last historical position
    m_history.pop_back();
    // Update bitboards
    update_bitboards();
    // Flip current player
    m_player *= -1;
}

