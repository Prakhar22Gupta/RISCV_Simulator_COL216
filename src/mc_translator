#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

using namespace std;

// Function to sign-extend immediate values
int32_t signExtend(int32_t value, int bits) {
    int32_t signBit = 1 << (bits - 1);
    return (value & (signBit - 1)) - (value & signBit);
}

// Manual RISC-V instruction decoder
string decodeRiscvInstruction(uint32_t instruction) {
    // Extract fields from the instruction
    uint8_t opcode = instruction & 0x7F;
    uint8_t funct3 = (instruction >> 12) & 0x7;
    uint8_t funct7 = (instruction >> 25) & 0x7F;
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm_i = (instruction >> 20) & 0xFFF;
    int32_t imm_s = ((instruction >> 25) & 0x7F) << 5 | ((instruction >> 7) & 0x1F);
    int32_t imm_b = ((instruction >> 31) & 0x1) << 12 | ((instruction >> 7) & 0x1) << 11 | ((instruction >> 25) & 0x3F) << 5 | ((instruction >> 8) & 0xF) << 1;
    int32_t imm_u = instruction & 0xFFFFF000;
    int32_t imm_j = ((instruction >> 31) & 0x1) << 20 | ((instruction >> 21) & 0x3FF) << 1 | ((instruction >> 20) & 0x1) << 11 | ((instruction >> 12) & 0xFF) << 12;

    // Sign-extend immediate values
    imm_i = signExtend(imm_i, 12);
    imm_s = signExtend(imm_s, 12);
    imm_b = signExtend(imm_b, 13);
    imm_j = signExtend(imm_j, 21);

    // Decode based on opcode and funct3
    switch (opcode) {
        case 0x03:  // Load instructions
            switch (funct3) {
                case 0x0: return "lb x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                case 0x1: return "lh x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                case 0x2: return "lw x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                case 0x3: return "ld x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                case 0x4: return "lbu x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                case 0x5: return "lhu x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                case 0x6: return "lwu x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
                default: return "Unknown load instruction";
            }
        case 0x13:  // Immediate instructions
            switch (funct3) {
                case 0x0: return "addi x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                case 0x1: return "slli x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                case 0x2: return "slti x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                case 0x3: return "sltiu x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                case 0x4: return "xori x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                case 0x5:
                    if (funct7 == 0x00) return "srli x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                    else if (funct7 == 0x20) return "srai x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                    else return "Unknown immediate instruction";
                case 0x6: return "ori x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                case 0x7: return "andi x" + to_string(rd) + ", x" + to_string(rs1) + ", " + to_string(imm_i);
                default: return "Unknown immediate instruction";
            }
        case 0x33:  // Register-Register instructions
            switch (funct3) {
                case 0x0:
                    if (funct7 == 0x00) return "add x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                    else if (funct7 == 0x20) return "sub x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                    else return "Unknown R-type instruction";
                case 0x1: return "sll x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                case 0x2: return "slt x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                case 0x3: return "sltu x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                case 0x4: return "xor x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                case 0x5:
                    if (funct7 == 0x00) return "srl x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                    else if (funct7 == 0x20) return "sra x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                    else return "Unknown R-type instruction";
                case 0x6: return "or x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                case 0x7: return "and x" + to_string(rd) + ", x" + to_string(rs1) + ", x" + to_string(rs2);
                default: return "Unknown R-type instruction";
            }
        case 0x63:  // Branch instructions
            switch (funct3) {
                case 0x0: return "beq x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm_b);
                case 0x1: return "bne x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm_b);
                case 0x4: return "blt x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm_b);
                case 0x5: return "bge x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm_b);
                case 0x6: return "bltu x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm_b);
                case 0x7: return "bgeu x" + to_string(rs1) + ", x" + to_string(rs2) + ", " + to_string(imm_b);
                default: return "Unknown branch instruction";
            }
        case 0x6F:  // Jump and Link (JAL)
            return "jal x" + to_string(rd) + ", " + to_string(imm_j);
        case 0x67:  // Jump and Link Register (JALR)
            return "jalr x" + to_string(rd) + ", " + to_string(imm_i) + "(x" + to_string(rs1) + ")";
        case 0x37:  // Load Upper Immediate (LUI)
            return "lui x" + to_string(rd) + ", " + to_string(imm_u);
        case 0x17:  // Add Upper Immediate to PC (AUIPC)
            return "auipc x" + to_string(rd) + ", " + to_string(imm_u);
        default: return "Unknown instruction";
    }
}

// Function to translate machine code to assembly
string translateMachineCode(const string& machineCode) {
    uint32_t instruction = stoul(machineCode, nullptr, 16);
    return decodeRiscvInstruction(instruction);
}

// Function to preprocess the file
void preprocessFile(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    vector<string> lines;
    string line;
    while (getline(inputFile, line)) {
        // Check if the line contains machine code (8-character hexadecimal)
        if (line.find_first_not_of("0123456789abcdefABCDEF") == string::npos && line.size() == 8) {
            // Translate machine code to assembly
            string assembly = translateMachineCode(line);
            lines.push_back(assembly);
        } else {
            // Treat the line as assembly code
            lines.push_back(line);
        }
    }
    inputFile.close();

    // Write the modified content back to the file
    ofstream outputFile(filename);
    if (!outputFile) {
        cerr << "Error: Could not open file " << filename << " for writing" << endl;
        return;
    }

    for (const string& modifiedLine : lines) {
        outputFile << modifiedLine << endl;
    }
    outputFile.close();
}

int main() {
    string filename = "input.txt";
    preprocessFile(filename);
    cout << "Machine code in " << filename << " has been translated to assembly instructions." << endl;
    return 0;
}
