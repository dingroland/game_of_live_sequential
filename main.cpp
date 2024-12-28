#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Timing.h"

using namespace std;

int NUM_ARGS = 7;


void parseArguments(int argc, char** argv, string& InputFile, string& OutputFile, int& generations, bool& measure) {
    // Check if the number of arguments is correct
    if (argc < NUM_ARGS) {
        cerr << "Error: Insufficient arguments.\nUsage: --load <file> --save <file> --generations <n> [--measure]\n";
        exit(EXIT_FAILURE);
    }

    for (int i=1; i<argc; i++) {
        string arg = argv[i];
        cout << "Argument " << i << ": " << arg << endl;
    }

}

int main(int argc, char** argv) {

    string InputFile, OutputFile;
    int generations;
    bool measure = false;

    parseArguments(argc, argv, InputFile, OutputFile, generations, measure);

    

    





    return 0;
}