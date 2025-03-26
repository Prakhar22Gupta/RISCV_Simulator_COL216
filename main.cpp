#include <iostream>
#include <fstream>
#include <string>
#include "mc_translator.hpp"  // assumes this header provides: std::string decodeMC(const std::string &machineCode)
using namespace std;

int main() {
    ifstream inFile("given_input.txt");
    if (!inFile) {
        cerr << "Error: Could not open given_input.txt" << endl;
        return 1;
    }
    
    ofstream outFile("input.txt");
    if (!outFile) {
        cerr << "Error: Could not open input.txt for writing" << endl;
        return 1;
    }
    
    string machineCode;
    while (getline(inFile, machineCode)) {
        // Skip empty lines if any
        if(machineCode.empty()) continue;
        
        // Decode the machine code using the mc_translator function.
        uint32_t instruction = stoul(machineCode, nullptr, 16);
        string assemblyInstruction = decodeRiscvInstruction(instruction);

        outFile << assemblyInstruction << "\n";
    }
    
    inFile.close();
    outFile.close();
    
    cout << "Machine code successfully translated into input.txt" << endl;
    return 0;
}