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
    int row = 0;
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



void printBoard(const vector<uint8_t>& board, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cout << (board[y * width + x] ? 'x' : '.');
        }
        cout << "\n";
    }
    cout << endl; // Add an extra line for clarity
}



int main(int argc, char** argv) {

    string InputFile, OutputFile;
    int generations;
    bool measure = false;

    parseArguments(argc, argv, InputFile, OutputFile, generations, measure);

    vector<uint8_t> board;
    int width, height;

    board = loadFile(InputFile, width, height);

    printBoard(board, width, height);


    

    





    return 0;
}