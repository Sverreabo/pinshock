// Provides the PinGame::Game class

#pragma once


#include "bitboard.hpp"
#include "../bitset/bitset.hpp"


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

    const uint64_t cache_clear_size = 10000 * 1000;
    BitSet::BitSet bitset_cache;
    
    int max_moves = 2;
    int cache_ignore_bottom = 8;


    int solveRecursive(board_t _board, int pins_left);

    void Construct(std::vector<std::vector<bool>>& bool_board, std::vector<std::vector<bool>>& legal_slots);
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
