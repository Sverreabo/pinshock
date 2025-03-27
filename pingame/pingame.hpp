// Provides the PinGame::Game class

#pragma once

#include <unordered_set>

#include "bitboard.hpp"



namespace PinGame
{

class Game
{
    board_t board;
    unsigned width, height;
    std::vector<board_t> solution;

    board_t full_board;
    unsigned n_slots;
    unsigned n_start_pins;

    board_t Cfull_board_2w_shift;
    unsigned double_width;
    board_t Cleft_mask;
    board_t Cright_mask_2l_shift;

    uint64_t num_iterations = 0;
    std::unordered_set<board_t> solved_cache;
    std::unordered_set<board_t> empty_cache; // solved_cache is reset to empty_cache every CACHE_CLEAR_SIZE calls to solve()
    const uint64_t CACHE_CLEAR_SIZE = 10000 * 1000;
    
    int MAX_MOVES = 2;
    int cache_ignore_bottom = 8;


    int solveRecursive(board_t _board, int pins_left);
public:
    Game(std::vector<std::vector<bool>>& bool_board);
    Game(std::vector<std::vector<bool>>& bool_board, std::vector<std::vector<bool>>& legal_slots);

    ~Game();
    
    void printGameBoard();
    void printSolution();
    board_t getBoard();
    std::vector<board_t> getMoves();
    std::vector<board_t> getMovesFromBoard(board_t board);
    void solve();


};


} // namespace PinGame
