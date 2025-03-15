#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <map>
#include <iomanip>
using namespace std;

class Processor {
protected:
    int cycleCount;
    vector<int> registers; // 32 registers
    vector<uint32_t> machine_code_storage; // loaded machine code from file
    vector<string> instruction_storage;    // loaded instructions from file

public:
    Processor() : cycleCount(0), registers(32, 0) {}    // initialize cycleCount and registers

    // Function to load instructions from file
    bool loadInstructions(const string &filename) {
        ifstream infile(filename);
        string line;
        
        if (!infile.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return false;
        }
        
        while (getline(infile, line)) {
            istringstream iss(line);
            string address_line_number, machine_code_of_line;
            // Read first two tokens; e.g., "0:" and "00000293"
            if (!(iss >> address_line_number >> machine_code_of_line))
                continue; 

            // Convert hex string to uint32_t
            uint32_t mcode = stoul(machine_code_of_line, nullptr, 16);
            
            // Read the rest of the line as the instruction (for printing)
            string instr;
            getline(iss, instr);
            size_t start = instr.find_first_not_of(" ");
            if (start != string::npos)
                instr = instr.substr(start);
            
            machine_code_storage.push_back(mcode);
            instruction_storage.push_back(instr);
        }
        
        infile.close();
        return !machine_code_storage.empty();
    }
    
    // Decode a single instruction (for printing)
    // Note: Operands are separated by spaces (no commas)
    string decodeInstruction(uint32_t code) {
        stringstream result;
        
        // Extract common fields
        uint32_t opcode = code & 0x7F;
        uint32_t rd = (code >> 7) & 0x1F;
        uint32_t rs1 = (code >> 15) & 0x1F;
        uint32_t rs2 = (code >> 20) & 0x1F;
        uint32_t funct3 = (code >> 12) & 0x7;
        uint32_t funct7 = (code >> 25) & 0x7F;
        
        // helper for register naming
        auto regName = [](uint32_t reg) -> string {
            return "x" + to_string(reg);
        };
        
        switch(opcode) {
            case 0x33: { // R-type
                string op;
                switch(funct3) {
                    case 0x0:
                        op = (funct7 == 0x00) ? "add" :
                             (funct7 == 0x01) ? "mul" :
                             (funct7 == 0x20) ? "sub" : "unknown_r_type";
                        break;
                    case 0x1:
                        op = (funct7 == 0x00) ? "sll" :
                             (funct7 == 0x01) ? "mulh" : "unknown_r_type";
                        break;
                    case 0x2:
                        op = (funct7 == 0x00) ? "slt" :
                             (funct7 == 0x01) ? "mulhsu" : "unknown_r_type";
                        break;
                    case 0x3:
                        op = (funct7 == 0x00) ? "sltu" :
                             (funct7 == 0x01) ? "mulhu" : "unknown_r_type";
                        break;
                    case 0x4:
                        op = (funct7 == 0x00) ? "xor" :
                             (funct7 == 0x01) ? "div" : "unknown_r_type";
                        break;
                    case 0x5:
                        op = (funct7 == 0x00) ? "srl" :
                             (funct7 == 0x01) ? "divu" :
                             (funct7 == 0x20) ? "sra" : "unknown_r_type";
                        break;
                    case 0x6:
                        op = (funct7 == 0x00) ? "or" :
                             (funct7 == 0x01) ? "rem" : "unknown_r_type";
                        break;
                    case 0x7:
                        op = (funct7 == 0x00) ? "and" :
                             (funct7 == 0x01) ? "remu" : "unknown_r_type";
                        break;
                    default:
                        op = "unknown_r_type";
                }
                result << op << " " << regName(rd) << " " << regName(rs1) << " " << regName(rs2);
                break;
            }
            case 0x13: { // I-type (e.g., addi)
                int32_t imm = ((int32_t)(code & 0xFFF00000)) >> 20;
                string op;
                switch(funct3) {
                    case 0x0: op = "addi"; break;
                    case 0x1: op = "slli"; break;
                    case 0x2: op = "slti"; break;
                    case 0x3: op = "sltiu"; break;
                    case 0x4: op = "xori"; break;
                    case 0x5:
                        op = ((code >> 25) & 0x7F) == 0x20 ? "srai" : "srli";
                        break;
                    case 0x6: op = "ori"; break;
                    case 0x7: op = "andi"; break;
                    default: op = "unknown_i_type";
                }
                result << op << " " << regName(rd) << " " << regName(rs1) << " " << imm;
                break;
            }
            case 0x03: { // Load instructions
                int32_t imm = ((int32_t)(code & 0xFFF00000)) >> 20;
                string op;
                switch(funct3) {
                    case 0x0: op = "lb"; break;
                    case 0x1: op = "lh"; break;
                    case 0x2: op = "lw"; break;
                    case 0x3: op = "ld"; break;
                    case 0x4: op = "lbu"; break;
                    case 0x5: op = "lhu"; break;
                    case 0x6: op = "lwu"; break;
                    default: op = "unknown_load";
                }
                result << op << " " << regName(rd) << " " << imm << "(" << regName(rs1) << ")";
                break;
            }
            case 0x23: { // Store instructions
                int32_t imm11_5 = (code >> 25) & 0x7F;
                int32_t imm4_0 = (code >> 7) & 0x1F;
                int32_t imm = (imm11_5 << 5) | imm4_0;
                if(imm11_5 & 0x40)
                    imm |= 0xFFFFF000;
                string op;
                switch(funct3) {
                    case 0x0: op = "sb"; break;
                    case 0x1: op = "sh"; break;
                    case 0x2: op = "sw"; break;
                    case 0x3: op = "sd"; break;
                    default: op = "unknown_store";
                }
                result << op << " " << regName(rs2) << " " << imm << "(" << regName(rs1) << ")";
                break;
            }
            case 0x63: { // Branch instructions
                int32_t imm12 = (code >> 31) & 0x1;
                int32_t imm11 = (code >> 7) & 0x1;
                int32_t imm10_5 = (code >> 25) & 0x3F;
                int32_t imm4_1 = (code >> 8) & 0xF;
                int32_t imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
                if(imm12)
                    imm |= 0xFFFFE000;
                string op;
                switch(funct3) {
                    case 0x0: op = "beq"; break;
                    case 0x1: op = "bne"; break;
                    case 0x4: op = "blt"; break;
                    case 0x5: op = "bge"; break;
                    case 0x6: op = "bltu"; break;
                    case 0x7: op = "bgeu"; break;
                    default: op = "unknown_branch";
                }
                result << op << " " << regName(rs1) << " " << regName(rs2) << " " << imm;
                break;
            }
            case 0x6F: { // JAL
                int32_t imm20 = (code >> 31) & 0x1;
                int32_t imm19_12 = (code >> 12) & 0xFF;
                int32_t imm11 = (code >> 20) & 0x1;
                int32_t imm10_1 = (code >> 21) & 0x3FF;
                int32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
                if(imm & (1 << 20))
                    imm -= (1 << 21);
                result << "jal " << regName(rd) << " " << imm;
                break;
            }
            case 0x67: { // JALR
                int32_t imm = ((int32_t)(code & 0xFFF00000)) >> 20;
                result << "jalr " << regName(rd) << " " << regName(rs1) << " " << imm;
                break;
            }
            case 0x37: { // LUI
                uint32_t imm = code & 0xFFFFF000;
                result << "lui " << regName(rd) << " 0x" << hex << (imm >> 12) << dec;
                break;
            }
            case 0x17: { // AUIPC
                uint32_t imm = code & 0xFFFFF000;
                result << "auipc " << regName(rd) << " 0x" << hex << (imm >> 12) << dec;
                break;
            }
            case 0x73: {
                if(code == 0x00000073)
                    result << "ecall";
                else if(code == 0x00100073)
                    result << "ebreak";
                else
                    result << "unknown_system";
                break;
            }
            case 0x0F: {
                if(funct3 == 0)
                    result << "fence";
                else if(funct3 == 1)
                    result << "fence.i";
                else
                    result << "unknown_fence";
                break;
            }
            default:
                result << "unknown_instruction_0x" << hex << opcode;
        }
        return result.str();
    }
    
    // Decode all instructions in instruction_storage
    void decodeAllInstructions() {
        for (size_t i = 0; i < machine_code_storage.size(); i++) {
            instruction_storage[i] = decodeInstruction(machine_code_storage[i]);
        }
    }
    
    // (Optional) Display decoded instructions for debugging
    void displayInstructions() {
        cout << "Address\tMachine Code\tInstruction" << endl;
        cout << "-------\t-----------\t-----------" << endl;
        for (size_t i = 0; i < machine_code_storage.size(); i++) {
            cout << i << ":\t0x" 
                 << hex << setw(8) << setfill('0') << machine_code_storage[i] << dec 
                 << "\t" << instruction_storage[i] << endl;
        }
    }
};

#endif // PROCESSOR_HPP
