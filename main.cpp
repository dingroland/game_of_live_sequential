#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Timing.h"

using namespace std;

int NUM_ARGS = 7;


void parseArguments(int argc, char** argv, string& inputFile, string& outputFile, int& generations, bool& measure) {
    // Check if the number of arguments is correct
    if (argc < NUM_ARGS) {
        cerr << "Error: Insufficient arguments.\nUsage: --load <file> --save <file> --generations <n> [--measure]\n";
        exit(EXIT_FAILURE);
    }

    for (int i=1; i<argc; i++) {
        string arg = argv[i];
        if (arg == "--load" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "--save" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--generations" && i + 1 < argc) {
            generations = stoi(argv[++i]);
        } else if (arg == "--measure") {
            measure = true;
        } else {
            cerr << "Error: Invalid argument \"" << arg << "\"\n";
            exit(EXIT_FAILURE);
        }
    }
    if (inputFile.empty() || outputFile.empty() || generations <= 0) {
        cerr << "Error: Invalid Arguments. Ensure Input File is populated and Generations are greater than 0.\n";
        exit(EXIT_FAILURE);
    }
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
    vector<uint8_t> next_board = board; // Temporary board to store the next state

    for (int gen = 0; gen < generations; ++gen) {
        cout << "Generation " << gen << ":\n";
        printBoard(board, width, height);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Count alive neighbors
                int alive_neighbours = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue; // Skip the current cell

                        // Wrap around edges (toroidal array)
                        int nx = (x + dx + width) % width;
                        int ny = (y + dy + height) % height;

                        // Increment alive neighbor count
                        alive_neighbours += board[ny * width + nx];
                    }
                }

                // Calculate the index of the current cell
                int idx = y * width + x;

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
            }
        }

        // Swap boards for the next generation
        board.swap(next_board);
    }
}




int main(int argc, char** argv) {

    string InputFile, OutputFile;
    int generations;
    bool measure = false;

    parseArguments(argc, argv, InputFile, OutputFile, generations, measure);

    vector<uint8_t> board;
    int width, height;

    board = loadFile(InputFile, width, height);

    cout << "Initial Board:\n";
    printBoard(board, width, height);


    runGame(board, width, height, generations);



    saveFile(OutputFile, board, width, height);

    return 0;
}