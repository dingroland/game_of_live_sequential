#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include "Timing.h"

using namespace std;


// Argument Parsing Function
void parseArguments(int argc, char** argv, string& inputFile, string& outputFile, int& generations, int& threads, bool& measure, bool& omp) {
    unordered_map<string, string> args;
    for (int i = 1; i < argc; i += 2) {
        string key = argv[i];
        if (i + 1 < argc)
            args[key] = argv[i + 1];
        else
            args[key] = "";
    }

    if (args.find("--load") != args.end()) inputFile = args["--load"];
    if (args.find("--save") != args.end()) outputFile = args["--save"];
    if (args.find("--generations") != args.end()) generations = stoi(args["--generations"]);
    if (args.find("--measure") != args.end()) measure = true;
    if (args.find("--mode") != args.end() && args["--mode"] == "omp") omp = true;
    if (args.find("--threads") != args.end() && omp) threads = stoi(args["--threads"]);

    if (inputFile.empty() || outputFile.empty() || generations <= 0) {
        cerr << "Error: Invalid arguments provided.\n";
        exit(EXIT_FAILURE);
    }
}


// Load File 
vector<uint8_t> loadFile(const string& inputFile, int& width, int& height) {
    ifstream file(inputFile);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << inputFile << "\n";
        exit(EXIT_FAILURE);
    }
    file >> width;
    file.ignore(1);
    file >> height;

    vector<uint8_t> board(width * height);
    for (int y = 0; y < height; ++y) {
        string line;
        file >> line;
        for (int x = 0; x < width; ++x) {
            board[y * width + x] = (line[x] == 'x');
        }
    }
    return board;
}

// Save File 
void saveFile(const string& outputFile, const vector<uint8_t>& board, int width, int height) {
    ofstream file(outputFile);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << outputFile << "\n";
        exit(EXIT_FAILURE);
    }
    file << width << "," << height << "\n";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            file.put(board[y * width + x] ? 'x' : '.');
        }
        file.put('\n');
    }
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


// Sequential Game of Life Logic
void runGame(vector<uint8_t>& board, int width, int height, int generations) {
    vector<uint8_t> next_board(width * height);

    for (int gen = 0; gen < generations; ++gen) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int alive_neighbours = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = (x + dx + width) % width;
                        int ny = (y + dy + height) % height;
                        alive_neighbours += board[ny * width + nx];
                    }
                }
                int idx = y * width + x;
                next_board[idx] = (alive_neighbours == 3) || (board[idx] == 1 && alive_neighbours == 2);
            }
        }
        board.swap(next_board);
    }
}


// Parallel 
#include <omp.h>
void runGameOMP(vector<uint8_t>& board, int width, int height, int generations, int threads) {
    vector<uint8_t> next_board(width * height);
    omp_set_num_threads(threads);

    for (int gen = 0; gen < generations; ++gen) {
        #pragma omp parallel for collapse(2) schedule(static)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int alive_neighbours = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = (x + dx + width) % width;
                        int ny = (y + dy + height) % height;
                        alive_neighbours += board[ny * width + nx];
                    }
                }
                int idx = y * width + x;
                next_board[idx] = (alive_neighbours == 3) || (board[idx] == 1 && alive_neighbours == 2);
            }
        }
        board.swap(next_board);
    }
}


// Main Function
int main(int argc, char** argv) {
    string inputFile, outputFile;
    int generations = 0;
    int threads = 1;
    bool measure = false;
    bool omp = false;

    parseArguments(argc, argv, inputFile, outputFile, generations, threads, measure, omp);

    if (measure) {
        Timing* timing = Timing::getInstance();
        timing->startSetup();
    }

    int width, height;
    vector<uint8_t> board = loadFile(inputFile, width, height);

    if (measure) {
        Timing* timing = Timing::getInstance();
        timing->stopSetup();
        timing->startComputation();
    }

    // Run either sequential or parallel based on flag
    if (omp) {
        cout << "Running in Parallel Mode with " << threads << " threads.\n";
        runGameOMP(board, width, height, generations, threads);
    } else {
        cout << "Running in Sequential Mode.\n";
        runGame(board, width, height, generations);
    }

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