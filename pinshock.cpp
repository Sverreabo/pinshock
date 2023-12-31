#include <iostream>
#include <vector>

#include <ctime>

#include <algorithm>
#include <random>

#include <unordered_set>

using namespace std;

auto RNG = default_random_engine{(long unsigned)time(NULL)};

using board_t = uint64_t; // The board is represented as a 64-bit bit-board. A one represents a pin and a zero an empty slot

#define WIDTH 7
#define HEIGHT 7
#define MAX_MOVES 2

#include "pinshock.h"

long num_iterations = 0;
long num_solutions = 0;
long empty_cache_size = 0;

// Options
int cache_ignore_bottom = 8;
bool do_print_solutions = true;

const unsigned DOUBLE_WIDTH = WIDTH * 2; // Useful for some calculations
const unsigned SLOTS = WIDTH * HEIGHT;   // The total number of slots on the board
// const board_t FULL_BOARD = ((board_t)1 << SLOTS) - 1; // A board where every slot has a pin
const board_t FULL_BOARD = convert_board((bool[WIDTH][HEIGHT]){
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0},
});

const board_t FULL_BOARD_2W_SHIFT = FULL_BOARD << DOUBLE_WIDTH; // FULL_BOARD left-shifted by the double width of the board,
                                                                //  meaning that the board is moved two rows upwards

board_t solution[SLOTS]; // The current solution from the recursive call to solve() is stored as a list of boards
int lowest_depth = 3;
int n_start_pins;

unordered_set<board_t> solved_cache;
unordered_set<board_t> empty_cache; // solved_cache is reset to empty_cache every CACHE_CLEAR_SIZE calls to solve()
const long CACHE_CLEAR_SIZE = 10000 * 1000;

board_t generate_horizontal_mask(unsigned left_column, unsigned right_column)
{
    board_t result = 0;

    for (unsigned i = SLOTS; i > 0; i--)
    {
        unsigned column = (i - 1) % WIDTH;
        if (column < left_column && column >= right_column)
        {
            result |= 1;
        }
        result <<= 1;
    }

    result >>= 1;

    return result;
}

const board_t LEFT_MASK = generate_horizontal_mask(WIDTH, 2) & FULL_BOARD;
const board_t RIGHT_MASK_2L_SHIFT = (generate_horizontal_mask(WIDTH - 2, 0) & FULL_BOARD) << 2;

void print_binary(uint64_t n)
{
    string binary;
    for (unsigned i = 0; i < 64; i++)
    {
        binary.insert(binary.begin(), (n & 1 ? '1' : '0'));
        n >>= 1;
    }
    cout << binary << "\n";
}

void print_board(board_t board)
{
    string output;
    for (unsigned y = 0; y < HEIGHT; y++)
    {
        output.insert(output.begin(), '\n');
        for (unsigned x = 0; x < WIDTH; x++)
        {
            output.insert(output.begin(), ' ');
            output.insert(output.begin(), (board & 1 ? 'X' : '\''));
            board >>= 1;
        }
    }
    cout << output;
}

// Counts the number of pins in a board
int count_board(board_t board)
{
    int n = 0;
    while (board > 0)
    {
        if (board & 1)
        {
            n++;
        }
        board >>= 1;
    }
    return n;
}

board_t convert_board(bool bool_board[HEIGHT][WIDTH])
{
    board_t result = 0;
    for (unsigned y = 0; y < HEIGHT; y++)
    {
        for (unsigned x = 0; x < WIDTH; x++)
        {
            result <<= 1;
            result |= (bool_board[y][x] ? 1 : 0);
        }
    }
    return result;
}

vector<board_t> get_moves(board_t board)
{
    vector<board_t> moves;

    board_t board_lshift = board << WIDTH;
    board_t board_2lshift = board_lshift << WIDTH;

    // Downwards, eg. when a pin is above another pin which is above an empty slot
    board_t result = board & board_lshift & (~board_2lshift) & FULL_BOARD_2W_SHIFT;
    if (result != 0)
    {
        result >>= DOUBLE_WIDTH;
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {

                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = board ^ move_slot ^ (move_slot << WIDTH) ^ (move_slot << DOUBLE_WIDTH);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }

    // Upwards, eg. when a pin is below another pin which is below an empty slot
    result = (~board) & board_lshift & board_2lshift & (FULL_BOARD);

    if (result != 0)
    {
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {
                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = board ^ move_slot ^ (move_slot >> WIDTH) ^ (move_slot >> DOUBLE_WIDTH);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }

    // Preparing variables for sideways movement
    board_lshift = board << 1;
    board_2lshift = board_lshift << 1;

    // Right, eg. when a pin has another pin to its right which has an empty slot to its right
    result = board & board_lshift & (~board_2lshift) & RIGHT_MASK_2L_SHIFT;
    if (result != 0)
    {
        result >>= 2;
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {
                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = board ^ move_slot ^ (move_slot << 1) ^ (move_slot << 2);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }
    // Left, eg. when a pin has another pin to its left which has an empty pin to its left
    result = (~board) & board_lshift & board_2lshift & LEFT_MASK;

    if (result != 0)
    {
        unsigned position = 0;
        while (result != 0)
        {
            if (result & 1)
            {
                board_t move_slot = ((board_t)1 << position); // The slot the pin will be moved to
                board_t new_board = board ^ move_slot ^ (move_slot >> 1) ^ (move_slot >> 2);

                moves.push_back(new_board);
            }
            position++;
            result >>= 1;
        }
    }

    return moves;
}

void print_solution(int depth = 1, int start = n_start_pins)
{
    for (int i = start; i >= depth; i--)
    {
        cout << "--------------\n";
        print_board(solution[i]);
    }
}

void solve(board_t board, int depth)
{
    if (num_iterations % CACHE_CLEAR_SIZE == 0)
    {
        cout << "Cache cleared\n";
        solved_cache = unordered_set<board_t>(empty_cache);
    }
    num_iterations++;

    vector<board_t> moves = get_moves(board);
    solution[depth] = board;

    if (depth < lowest_depth)
    {
        lowest_depth = depth;
        cout << "Depth: " << depth << "\n";
        if (do_print_solutions)
        {
            print_solution(depth);
        }
    }

    if (depth == 1)
    {
        num_solutions++;
    }

    shuffle(moves.begin(), moves.end(), RNG);
    size_t size = moves.size();
    size = min(size, (size_t)MAX_MOVES);

    for (size_t i = 0; i < size; i++)
    {
        board_t new_board = moves[i];
        if (!(depth - 1 > cache_ignore_bottom) || !solved_cache.contains(new_board))
        {

            solve(new_board, depth - 1);
        }
    }

    if (depth > 18)
    {
        empty_cache_size++;
        empty_cache.emplace(board);
    }
    if (depth > cache_ignore_bottom)
    {
        solved_cache.emplace(board);
    }
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        int number = atoi(argv[i]);
        switch (i)
        {
        case 1:
            cache_ignore_bottom = number;
            cout << "Set cache_ignore_bottom=" << cache_ignore_bottom << "\n";
            break;

        case 2:
            do_print_solutions = (bool)number;
            cout << "Set do_print_solutions=" << do_print_solutions << "\n";
            break;
        default:
            cout << "(Number ignored" << number << ")\n";
            break;
        }
    }

    empty_cache.reserve(CACHE_CLEAR_SIZE * 4);

    // bool bool_board[HEIGHT][WIDTH] = {
    //     {0, 1, 1, 1, 1, 1, 1},
    //     {1, 1, 1, 1, 1, 1, 1},
    //     {1, 1, 1, 1, 1, 1, 1},
    //     {1, 1, 1, 1, 1, 1, 1},
    //     {1, 1, 1, 1, 1, 1, 1},
    // };

    bool bool_board[HEIGHT][WIDTH] = {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
    };

    board_t board = convert_board(bool_board);

    n_start_pins = count_board(board);

    solve(board, n_start_pins);
    cout << "# of iterations: " << num_iterations << "\n";
    cout << "# of solutions: " << num_solutions << "\n";
    cout << "empty_cache_size: " << empty_cache_size << "\n";
}