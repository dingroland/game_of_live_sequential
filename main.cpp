#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Timing.h"

using namespace std;

int NUM_ARGS = 7;


void parseArguments(int argc, char** argv, string& inputFile, string& outputFile, int& generations, int& threads, bool& measure, bool& omp) {
    // Check if the number of arguments is correct
    if (argc < NUM_ARGS) {
        cerr << "Error: Insufficient arguments.\nUsage: --load <file> --save <file> --generations <n> [--measure] [--mode <seq|omp> --threads <n>]\n";
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--mode" && i + 1 < argc) {
            string mode = argv[++i];
            if (mode == "omp") {
                omp = true;
            } else if (mode == "seq") {
                omp = false;
            } else {
                cerr << "Error: Invalid mode \"" << mode << "\". Use \"seq\" or \"omp\".\n";
                exit(EXIT_FAILURE);
            }
        } else if (arg == "--threads" && i + 1 < argc) {
            if (omp) {  // Ensure threads are only set when in omp
                threads = stoi(argv[++i]);
                if (threads <= 0) {
                    cerr << "Error: Invalid thread count \"" << threads << "\". Must be > 0.\n";
                    exit(EXIT_FAILURE);
                }
            } else {
                cerr << "Error: \"--threads\" option is only valid with \"--mode omp\".\n";
                exit(EXIT_FAILURE);
            }
        } else if (arg == "--load" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "--save" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--generations" && i + 1 < argc) {
            generations = stoi(argv[++i]);
        } else if (arg == "--measure") {
            measure = true;
        } else {
            cerr << "Error: Invalid argument \"" << arg << "\".\n";
            exit(EXIT_FAILURE);
        }
    }

    // Final argument validation
    if (inputFile.empty() || outputFile.empty() || generations <= 0) {
        cerr << "Error: Invalid Arguments. Ensure Input File and Output File are provided, and Generations > 0.\n";
        exit(EXIT_FAILURE);
    }

    if (omp && threads <= 0) {
        cerr << "Error: Threads must be set and > 0 for OpenMP mode.\n";
        exit(EXIT_FAILURE);
    }
}



#include <iostream>
#include <string>

void printArguments(int argc, char** argv) {
    std::cout << "Received Arguments:\n";
    for (int i = 0; i < argc; i++) {
        std::cout << "  argv[" << i << "]: " << argv[i] << "\n";
    }
    std::cout << "-----------------------\n";
}

void validateArguments(const std::string& inputFile, const std::string& outputFile, int generations, int threads, bool omp) {
    std::cout << "Validated Parameters:\n";
    std::cout << "  Input File: " << (inputFile.empty() ? "Not Provided" : inputFile) << "\n";
    std::cout << "  Output File: " << (outputFile.empty() ? "Not Provided" : outputFile) << "\n";
    std::cout << "  Generations: " << (generations > 0 ? std::to_string(generations) : "Invalid") << "\n";

    if (omp) {
        std::cout << "  Mode: Parallel (OpenMP)\n";
        std::cout << "  Threads: " << (threads > 0 ? std::to_string(threads) : "Invalid") << "\n";
    } else {
        std::cout << "  Mode: Sequential\n";
    }

    if (inputFile.empty() || outputFile.empty() || generations <= 0 || (omp && threads <= 0)) {
        std::cerr << "Error: Invalid arguments detected.\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "-----------------------\n";
}




vector<uint8_t> loadFile(const string& inputFile, int& width, int& height) {
    ifstream file(inputFile);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << inputFile << "\n";
        exit(EXIT_FAILURE);
    }

    // Read board dimensions
    file >> width;
    file.ignore(1); // Skip comma
    file >> height;

    vector<uint8_t> board(width * height, 0);

    // Read board state
    string line;
    int row = -1; // Start at -1 to skip the dimensions line
    while (getline(file, line)) {
        if (row >= height) break;
        for (int col = 0; col < width && col < line.size(); ++col) {
            board[row * width + col] = (line[col] == 'x');
        }
        row++;
    }

    file.close();
    return board;
}


void saveFile(const string& outputFile, const vector<uint8_t>& board, int width, int height) {
    ofstream file(outputFile);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << outputFile << "\n";
        exit(EXIT_FAILURE);
    }

    file << width << "," << height << "\n";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            file << (board[y * width + x] ? 'x' : '.');
        }
        file << "\n";
    }

    file.close();
}


void printBoard(const vector<uint8_t>& board, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cout << (board[y * width + x] ? 'x' : '.');
        }
        cout << "\n";
    }
    cout << endl;
}


void runGame(vector<uint8_t>& board, int width, int height, int generations) {
    

    for (int gen = 0; gen < generations; ++gen) {
        vector<uint8_t> next_board = board; // Temporary board to store the next state

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Count alive neighbors
                int alive_neighbours = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue; // Skip the current cell

                        // Wrap around edges 
                        int nx = (x + dx + width) % width;
                        int ny = (y + dy + height) % height;

                        // Increment alive neighbor count
                        alive_neighbours += board[ny * width + nx];
                    }
                }

                // Calculate the index of the current cell
                int idx = y * width + x;
                
                // Optimized for performance: Reduces branching, minimizing CPU mispredictions

                next_board[idx] = (alive_neighbours == 3) || (board[idx] == 1 && alive_neighbours == 2);
                
                
                /*
                // Apply Game of Life rules
                if (board[idx] == 1) { 
                    if (alive_neighbours < 2 || alive_neighbours > 3) {
                        next_board[idx] = 0;
                        }
                        else if (alive_neighbours == 2 || alive_neighbours == 3) {
                            next_board[idx] = 1;
                        }

                } else { // Cell is dead
                    next_board[idx] = (alive_neighbours == 3) ? 1 : 0;
                }
                
                
                */
                
            }
        }

        // Swap boards for the next generation
        board.swap(next_board);
    }
}


int main(int argc, char** argv) {

    string inputFile, outputFile;
    int generations;
    int threads = 1;
    bool measure = false;
    bool omp = false;


    printArguments(argc, argv);

    parseArguments(argc, argv, inputFile, outputFile, generations, threads, measure, omp);


    validateArguments(inputFile, outputFile, generations, threads, omp);

    if (measure) {
        Timing* timing = Timing::getInstance();
        timing->startSetup();
    }

    vector<uint8_t> board;
    int width, height;

    board = loadFile(inputFile, width, height);
    if (measure) {
        Timing* timing = Timing::getInstance();
        timing->stopSetup();
        timing->startComputation();
    }
    
    runGame(board, width, height, generations);

    if (measure) {
        Timing* timing = Timing::getInstance();
        timing->stopComputation();
        timing->startFinalization();
    }

    saveFile(outputFile, board, width, height);


    if (measure) {
        Timing* timing = Timing::getInstance();
        timing->stopFinalization();
        timing->print();
    }

    return 0;
}