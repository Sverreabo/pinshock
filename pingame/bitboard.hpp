// Functions for dealing with bit-boards for the pin-game

#pragma once

#include <cstdint>
#include <vector>

namespace PinGame
{

using board_t = uint64_t; // The board is represented as a 64-bit bit-board. A one represents a pin and a zero an empty slot
constexpr unsigned SIZE_BITS_BOARD_T = (sizeof(board_t) * 8);


bool fitsInBoard(unsigned height, unsigned width);
board_t convertBoard(std::vector<std::vector<bool>> bool_board, unsigned height, unsigned width);
void printBoard(board_t board, unsigned height, unsigned width);
board_t genHorizontalMask(unsigned left_col, unsigned right_col, unsigned height, unsigned width);
unsigned countBoard(board_t board);

} // namespace PinGame
