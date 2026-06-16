#include <iostream>
#include <stdexcept>

#include <algorithm>
#include <random>

#include "pingame.hpp"

using namespace std;

namespace PinGame
{

random_device os_seed;

uint64_t rng_state = os_seed();

inline uint64_t fast_rng() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}


Game::Game(vector<vector<bool>>& bool_board)
{
    vector<vector<bool>> legal_slots;
    Construct(bool_board, legal_slots);
}

Game::Game(vector<vector<bool>>& bool_board, vector<vector<bool>>& legal_slots)
{
    Construct(bool_board, legal_slots);
}

void Game::Construct(vector<vector<bool>>& bool_board, vector<vector<bool>>& legal_slots)
{
    height = bool_board.size();
    width = bool_board.at(0).size();

    if (!fitsInBoard(height, width)) {
        throw invalid_argument("Height and width are too large for board_t!");
    }
    n_slots = height * width;
    
    solution.resize(n_slots);
    moveBuffers.resize(n_slots);
    
    board = convertBoard(bool_board, height, width);
    n_start_pins = countBoard(board);


    if (legal_slots.empty()) {
        full_board = ((board_t)1 << n_slots) - 1;
    } else {
        if (legal_slots.size() != height || legal_slots.at(0).size() != width) {
            throw invalid_argument("Argument legal_slots is not the same size as bool_board!");
        }
        full_board = convertBoard(legal_slots, height, width);
    }

    double_width = width * 2;
    Cfull_board_2w_shift = full_board << double_width;

    Cleft_mask = (genHorizontalMask(0,width-3, height, width) & full_board);
    Cright_mask_2l_shift = (genHorizontalMask(2, width-1, height, width) & full_board) << 2;
}

Game::~Game()
{
}


void Game::printGameBoard()
{
    cout << height << "x" << width << "\n";
    printBoard(board, height, width);
}

void Game::printSolution() {
    for (unsigned i = n_start_pins; i > 0; i--) {
        board_t _board = solution.at(i);
        cout << "----------------\n";
        printBoard(_board, height, width);
    }
}

void Game::printSolution(unsigned columns) {

    // Naming the columns and rows that are made out of gameboards 'big-' to 
    //  seperate them from cols and rows made out of 1 and 0 in each gameboard
    const unsigned N_BIG_COLS = columns;
    const unsigned N_BIG_ROWS = (n_start_pins % N_BIG_COLS == 0) ? 
                        (n_start_pins / N_BIG_COLS) : (n_start_pins / N_BIG_COLS + 1);

    const std::string separator((width*2 + 3) * N_BIG_COLS - 1, '-');
    
    for (unsigned big_row = 0; big_row < N_BIG_ROWS; big_row++) {
        for (unsigned row = 0; row < height; row++) {
            for (unsigned big_col = 0; big_col < N_BIG_COLS; big_col++) {

                    unsigned pos = (N_BIG_COLS * big_row + big_col);
                    if (pos >= n_start_pins) {
                        printRowFromBoard((board_t)0, row, width, height);
                    } else {
                        const unsigned index = n_start_pins - pos;
                        printRowFromBoard(solution.at(index), row, width, height);
                    }
                    cout << " | ";

                }
            cout << "\n";
        }
        
        cout << separator << "\n";
    }
}

vector<board_t> Game::getSolution() {
    vector<board_t> result;
    for (board_t& board : solution) {
        result.push_back(board);
    }
    return result;
}

board_t Game::getBoard()
{
    return board;
}


vector<board_t> Game::getMoves() {
    std::vector<board_t> moves;
    getMovesFromBoard(board, moves);
    return moves;
}

void Game::getMovesFromBoard(board_t _board, std::vector<board_t>& moves)
{
    moves.clear();

    board_t board_lshift = _board << width;
    board_t board_2lshift = board_lshift << width;

    // Downwards, eg. when a pin is above another pin which is above an empty slot
    board_t result = _board & board_lshift & (~board_2lshift) & Cfull_board_2w_shift;
    if (result != 0)
    {
        result >>= double_width;
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {

                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = _board ^ move_slot ^ (move_slot << width) ^ (move_slot << double_width);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }

    // Upwards, eg. when a pin is below another pin which is below an empty slot
    result = (~_board) & board_lshift & board_2lshift & (full_board);
    if (result != 0)
    {
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {
                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = _board ^ move_slot ^ (move_slot >> width) ^ (move_slot >> double_width);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }

    // Preparing variables for sideways movement
    board_lshift = _board << 1;
    board_2lshift = board_lshift << 1;

    // Right, eg. when a pin has another pin to its right which has an empty slot to its right
    result = _board & board_lshift & (~board_2lshift) & Cright_mask_2l_shift;
    if (result != 0)
    {
        result >>= 2;
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {
                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = _board ^ move_slot ^ (move_slot << 1) ^ (move_slot << 2);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }
    // Left, eg. when a pin has another pin to its left which has an empty pin to its left
    result = (~_board) & board_lshift & board_2lshift & Cleft_mask;

    if (result != 0)
    {
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {
                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = _board ^ move_slot ^ (move_slot >> 1) ^ (move_slot >> 2);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }
}

void Game::solve()
{
    num_iterations = 0;
    
    bitset_cache.init(cache_clear_size);

    int n_tries_current = 0;
    const int TRIES_UNTIL_CLEAR = 3;

    while (solveRecursive(board, n_start_pins) == 0) {
        n_tries_current++;
        if (n_tries_current >= TRIES_UNTIL_CLEAR) {
            bitset_cache.clear();
            n_tries_current = 0;
        }
    }
}


// Returns 1 if solution found
int Game::solveRecursive(board_t _board, int pins_left)
{
    if (cache_usage > cache_clear_size)
    {
        bitset_cache.clear();
        cache_usage = 0;
    }
    num_iterations++;

    auto& moves = moveBuffers.at(pins_left);
    getMovesFromBoard(_board, moves);

    solution[pins_left] = _board;

    if (pins_left == 1)
    {
        return 1;
    }

    size_t size = moves.size();
    size = min(size, (size_t)max_moves);

    for (size_t i = 0; i < size; i++)
    {
        uint64_t random_i = fast_rng() % moves.size();
        board_t new_board = moves[random_i];
        moves[random_i] = moves.back();
        moves.pop_back();

        if (!(pins_left - 1 > cache_ignore_bottom) || !bitset_cache.get(new_board))
        {

            if (solveRecursive(new_board, pins_left - 1)) return 1;
        }
    }

    if (pins_left > cache_ignore_bottom)
    {
        bitset_cache.set(_board);
        cache_usage++;
    }

    return 0;
}


void Game::set_cache_clear_size(uint64_t n) {
    cache_clear_size = n;
}

} // namespace PinGame
