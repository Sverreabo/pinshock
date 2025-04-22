#include <iostream>
#include <stdexcept>

#include <ctime>
#include <algorithm>
#include <random>

#include "pingame.hpp"

using namespace std;

namespace PinGame
{

auto RNG = default_random_engine{(long unsigned)time(NULL)};


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

board_t Game::getBoard()
{
    return board;
}


vector<board_t> Game::getMoves() {
    return getMovesFromBoard(board);
}

vector<board_t> Game::getMovesFromBoard(board_t _board)
{
    static vector<board_t> moves;
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

    return moves;
}

void Game::solve()
{
    num_iterations = 0;
    
    bitset_cache.init(cache_clear_size);

    while (solveRecursive(board, n_start_pins) == 0) {
        max_moves++;
    }
}

// Returns 1 if solution found
int Game::solveRecursive(board_t _board, int pins_left)
{
    if (num_iterations % cache_clear_size == 0)
    {
        cout << "Cache cleared\n";
        bitset_cache.clear();
    }
    num_iterations++;

    vector<board_t> moves = getMovesFromBoard(_board);
    solution[pins_left] = _board;

    if (pins_left == 1)
    {
        cout << "\nPins left: " << pins_left << "\n";
        return 1;
    }

    shuffle(moves.begin(), moves.end(), RNG);
    size_t size = moves.size();
    size = min(size, (size_t)max_moves);

    for (size_t i = 0; i < size; i++)
    {
        board_t new_board = moves[i];
        if (!(pins_left - 1 > cache_ignore_bottom) || !bitset_cache.get(new_board))
        {

            if (solveRecursive(new_board, pins_left - 1)) return 1;
        }
    }

    if (pins_left > cache_ignore_bottom)
    {
        bitset_cache.set(_board);
    }

    return 0;
}


void Game::set_cache_clear_size(uint64_t n) {
    cache_clear_size = n;
}

} // namespace PinGame
