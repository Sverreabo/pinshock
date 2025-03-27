#include "bitboard.hpp"
#include <iostream>

using namespace std;

namespace PinGame
{

bool fitsInBoard(unsigned height, unsigned width)
{
    return (width * height + 2*width) <= SIZE_BITS_BOARD_T;
}

board_t convertBoard(vector<vector<bool>> bool_board, unsigned height, unsigned width)
{
    board_t result = 0;

    for (unsigned y = 0; y < height; y++)
    {
        for (unsigned x = 0; x < width; x++)
        {
            result <<= 1;
            result |= (bool_board.at(y).at(x) ? 1 : 0);
        }
    }
    return result;
}


void printBoard(board_t board, unsigned height, unsigned width)
{
    unsigned pos = (width * height) - 1;
    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            bool bit = (board >> pos) & 0x1;
            cout << (bit ? "X " : "` ");
            pos -= 1;
        }

        cout << "\n";
    }
}



board_t genHorizontalMask(unsigned left_col, unsigned right_col, unsigned height, unsigned width)
{
    board_t row = 0;

    for (unsigned x = 0; x < width; x++) {
        if (x >= left_col && x <= right_col) {
            row |= 0x1;
        }
        row <<= 1;
    }
    row >>= 1;

    board_t result = 0;
    for (unsigned y = 0; y < height; y++) {
        result |= row << (y * width);
    }

    return result;
}

unsigned countBoard(board_t board)
{
    unsigned count = 0;
    while (board > 0) {
        if (board & 0x1) {
            count++;
        }
        board >>= 1;
    }

    return count;
}


} // namespace PinGame
