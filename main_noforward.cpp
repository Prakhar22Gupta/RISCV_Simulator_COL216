#include "NonForwardingProcessor.hpp"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    string filename = argv[1];
    int cycles = stoi(argv[2]);

    NonForwardingProcessor proc;
    if (!proc.loadInstructions(filename)) {
        return 1;
    }
    proc.runSimulation(cycles);
    
    return 0;
}
