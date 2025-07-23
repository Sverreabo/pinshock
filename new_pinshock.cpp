#include "pingame/pingame.hpp"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Config {
public:
    uint64_t cache_clear_size = 10000 * 1000;
    bool print_solution = true;
} global_config;

void apply_config(Config& config, PinGame::Game& game)
{
    // cache_clear_size
    game.set_cache_clear_size(config.cache_clear_size);
    
}


void load_config_from_args(int argc, char* argv[], struct Config& config) {

    char expect_arg = 0; // Keeps track of what argument will come next
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        switch (expect_arg)
        {
        case 'm': // --memory (sets cache_clear_size)
            {
                uint64_t memory = stoul(arg);
                // argument is in kB. Convert to number of buckets
                config.cache_clear_size = memory * (1000 / (u_int64_t)sizeof(PinGame::board_t));
                cout << "cache_clear_size = " << config.cache_clear_size << "\n";

                break;
            }


        default:
            break;
        }
        
        if (arg == "-m" || arg == "--memory") {
            expect_arg = 'm';
        }
        if (arg == "-q" || arg == "--quiet") {
            config.print_solution = false;
        }


    }
}

int main(int argc, char* argv[]) {

    vector<vector<bool>> bool_board = {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
    };
    vector<vector<bool>> legal_slots = {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
    };
    
    vector<vector<bool>> second_board = {
        {0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
    };
    
    auto game = PinGame::Game(bool_board, legal_slots);

    load_config_from_args(argc, argv, global_config);
    apply_config(global_config, game);

    game.solve();

    if (global_config.print_solution) game.printSolution();
}
