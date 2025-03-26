#include <iostream>
#include <fstream>
#include <string>
#include "mc_translator.hpp"  // assumes this header provides: std::string decodeMC(const std::string &machineCode)
using namespace std;

int main(){
    int total_instructions = 0;
    vector<string> instructions;
    fstream file;
    file.open("given_input.txt", ios::in);
    if (file.is_open()) {
    string input;
    while (getline(file, input)) {
      instructions.push_back(input);
      total_instructions++;
    }
    file.close();
    }
  
    vector<string> disassembled_instruction;
    for (int i = 0; i < total_instructions; i++) {
    string temp;
    temp = disassembler(total_instructions, instructions[i], i + 1);
    disassembled_instruction.push_back(temp);
    }
  
    for (int i = 0; i < total_instructions; i++) {
    if (disassembled_instruction[i].find("beq", 0) != string::npos ||
      disassembled_instruction[i].find("bne", 0) != string::npos ||
      disassembled_instruction[i].find("blt", 0) != string::npos ||
      disassembled_instruction[i].find("bge", 0) != string::npos ||
      disassembled_instruction[i].find("bltu", 0) != string::npos ||
      disassembled_instruction[i].find("bgeu", 0) != string::npos ||
      (disassembled_instruction[i].find("jal", 0) != string::npos &&
       disassembled_instruction[i].find("jalr", 0) == string::npos)) {
      
      size_t offset_pos = disassembled_instruction[i].rfind(',');
      if (offset_pos != string::npos) {
      string offset = sub_string(disassembled_instruction[i], offset_pos + 2, disassembled_instruction[i].size() - 1);
      disassembled_instruction[i] = sub_string(disassembled_instruction[i], 0, offset_pos + 1) + offset;
      }
    }
    }
  
    file.open("input.txt", ios::out);
    if (file.is_open()) {
    for (int i = 0; i < total_instructions; i++) {
      file << disassembled_instruction.at(i) << endl;
    }
    file.close();
    }
    return 0;
  }
  