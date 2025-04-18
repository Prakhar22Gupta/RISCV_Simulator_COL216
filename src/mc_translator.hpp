#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<cmath>
#include<cstdio>
#include<unordered_map>
using namespace std;

#define size_of_instruction_hex 8

string convert_to_binary(char num){
  string con ;
  if(num == '0') con = "0000" ;
  else if(num == '1') con = "0001" ;
  else if(num == '2') con = "0010" ;
  else if(num == '3') con = "0011" ;
  else if(num == '4') con = "0100" ;
  else if(num == '5') con = "0101" ;
  else if(num == '6') con = "0110" ;
  else if(num == '7') con = "0111" ;
  else if(num == '8') con = "1000" ;
  else if(num == '9') con = "1001" ;
  else if(num == 'a') con = "1010" ;
  else if(num == 'b') con = "1011" ;
  else if(num == 'c') con = "1100" ;
  else if(num == 'd') con = "1101" ;
  else if(num == 'e') con = "1110" ;
  else if(num == 'f') con = "1111" ;
  return con ;
}

int binary_to_decimal(int token, string binary){
  int decimal = 0;
  int max_pow = (int)pow(2, binary.size());

  for(int i = 0; i < binary.size(); i++){
    int radical = binary.size() - 1 - i;
    if(binary[i] == '0')
      decimal += 0;
    else 
      decimal += (int)pow(2, radical);
  }

  // For signed immediates in I/B-type instructions
  if(token == 1 && binary[0] == '1')
    decimal -= max_pow;
    
  return decimal;
}

string sub_string(string parent, int start, int end){
  string sub;
  for(int i = start; i <= end; i++) 
    sub += parent[i];
  return sub;
}

string R_format(const string& stringBinary){
  string instruction;
  
  string func7 = sub_string(stringBinary, 0, 6);
  string rs2   = sub_string(stringBinary, 7, 11);
  string rs1   = sub_string(stringBinary, 12, 16);
  string func3 = sub_string(stringBinary, 17, 19);
  string rd    = sub_string(stringBinary, 20, 24);
  
  if(func3 == "000" && func7 == "0000000") instruction += "add ";
  else if(func3 == "000" && func7 == "0100000") instruction += "sub ";
  else if(func3 == "100" && func7 == "0000000") instruction += "xor ";
  else if(func3 == "110" && func7 == "0000000") instruction += "or ";
  else if(func3 == "111" && func7 == "0000000") instruction += "and ";
  else if(func3 == "001" && func7 == "0000000") instruction += "sll ";
  else if(func3 == "101" && func7 == "0000000") instruction += "srl ";
  else if(func3 == "101" && func7 == "0100000") instruction += "sra ";
  else if(func3 == "000" && func7 == "0000001") instruction += "mul ";
  else if(func3 == "100" && func7 == "0000001") instruction += "div ";
  else if(func3 == "111" && func7 == "0000001") instruction += "rem ";
  else {
    instruction = "Invalid instruction";
    return instruction;
  }
  
  instruction += "x" + to_string(binary_to_decimal(0, rd));
  instruction += " x" + to_string(binary_to_decimal(0, rs1));
  instruction += " x" + to_string(binary_to_decimal(0, rs2));
  
  return instruction;
}

string I_format_immediate(const string& stringBinary){
  string instruction;
  
  string imm   = sub_string(stringBinary, 0, 11);
  string rs1   = sub_string(stringBinary, 12, 16);
  string func3 = sub_string(stringBinary, 17, 19);
  string rd    = sub_string(stringBinary, 20, 24);
  
  if(func3 == "000") {
    instruction += "addi ";
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(1, imm));
  }
  else if(func3 == "100") {
    instruction += "xori ";
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(1, imm));
  }
  else if(func3 == "110") {
    instruction += "ori ";
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(1, imm));
  }
  else if(func3 == "111") {
    instruction += "andi ";
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(1, imm));
  }
  else if(func3 == "001" && sub_string(imm, 0, 5) == "000000") {
    instruction += "slli ";
    imm = sub_string(imm, 6, 11);
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(0, imm));
  }
  else if(func3 == "101" && sub_string(imm, 0, 5) == "000000") {
    instruction += "srli ";
    imm = sub_string(imm, 6, 11);
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(0, imm));
  }
  else if(func3 == "101" && sub_string(imm, 0, 5) == "010000") {
    instruction += "srai ";
    imm = sub_string(imm, 6, 11);
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " x" + to_string(binary_to_decimal(0, rs1));
    instruction += " " + to_string(binary_to_decimal(0, imm));
  }
  else {
    instruction = "Invalid Instruction";
  }
  return instruction;
}

string I_format_load(const string& stringBinary){
  string instruction;
  string imm   = sub_string(stringBinary, 0, 11);
  string rs1   = sub_string(stringBinary, 12, 16);
  string func3 = sub_string(stringBinary, 17, 19);
  string rd    = sub_string(stringBinary, 20, 24);

  if(func3 == "000") instruction += "lb ";
  else if(func3 == "001") instruction += "lh ";
  else if(func3 == "010") instruction += "lw ";
  else if(func3 == "011") instruction += "ld ";
  else if(func3 == "100") instruction += "lbu ";
  else if(func3 == "101") instruction += "lhu ";
  else if(func3 == "110") instruction += "lwu ";
  else {
    instruction = "Invalid instruction";
    return instruction;
  }
  instruction += "x" + to_string(binary_to_decimal(0, rd));
  instruction += " " + to_string(binary_to_decimal(1, imm));
  instruction += " x" + to_string(binary_to_decimal(0, rs1));

  return instruction;
}

string S_format(const string& stringBinary){
  string instruction;
  string imm;
  imm += sub_string(stringBinary, 0, 6);
  string rs2   = sub_string(stringBinary, 7, 11);
  string rs1   = sub_string(stringBinary, 12, 16);
  string func3 = sub_string(stringBinary, 17, 19);
  imm += sub_string(stringBinary, 20, 24);
  if(func3 == "000") instruction += "sb ";
  else if(func3 == "001") instruction += "sh ";
  else if(func3 == "010") instruction += "sw ";
  else if(func3 == "011") instruction += "sd ";
  else {
    instruction = "Invalid Instruction";
    return instruction;
  }

  instruction += "x" + to_string(binary_to_decimal(0, rs2));
  instruction += " " + to_string(binary_to_decimal(1, imm));
  instruction += " x" + to_string(binary_to_decimal(0, rs1));

  return instruction;
}

string B_format(string stringBinary, int inst_no, int total_instructions){
  string instruction;
  string imm;
  imm += stringBinary[0];
  imm += stringBinary[24];
  imm += sub_string(stringBinary, 1, 6);
  imm += sub_string(stringBinary, 20, 23);
  imm += "0";

  string rs2   = sub_string(stringBinary, 7, 11);
  string rs1   = sub_string(stringBinary, 12, 16);
  string func3 = sub_string(stringBinary, 17, 19);

  if(func3 == "000") instruction += "beq ";
  else if(func3 == "001") instruction += "bne ";
  else if(func3 == "100") instruction += "blt ";
  else if(func3 == "101") instruction += "bge ";
  else if(func3 == "110") instruction += "bltu ";
  else if(func3 == "111") instruction += "bgeu ";

  if(inst_no + (binary_to_decimal(1, imm) / 4) > total_instructions ||
     inst_no + (binary_to_decimal(1, imm) / 4) < 0)
    instruction = "The offset is out of bounds";
  else {
    instruction += "x" + to_string(binary_to_decimal(0, rs1));
    instruction += " x" + to_string(binary_to_decimal(0, rs2));
    instruction += " " + to_string(binary_to_decimal(1, imm));
  }
  return instruction;
}

string jal(string stringBinary, int inst_no, int total_instructions){
  string instruction;
  string imm;
  imm += stringBinary[0];
  imm += sub_string(stringBinary, 12, 19);
  imm += stringBinary[11];
  imm += sub_string(stringBinary, 1, 10);
  imm += "0";

  string rd = sub_string(stringBinary, 20, 24);
  if(inst_no + (binary_to_decimal(1, imm) / 4) > total_instructions ||
     inst_no + (binary_to_decimal(1, imm) / 4) < 0)
    instruction = "The offset is out of bounds";
  else {
    instruction += "jal ";
    instruction += "x" + to_string(binary_to_decimal(0, rd));
    instruction += " " + to_string(binary_to_decimal(1, imm));
  }
  return instruction;
}

string jalr(const string& stringBinary){
  string instruction;
  string imm   = sub_string(stringBinary, 0, 11);
  string rs1   = sub_string(stringBinary, 12, 16);
  string func3 = sub_string(stringBinary, 17, 19);
  string rd    = sub_string(stringBinary, 20, 24);
  instruction += "jalr x0 ";
  instruction += "x" + to_string(binary_to_decimal(0, rs1));
  instruction += " " + to_string(binary_to_decimal(1, imm));
  return instruction;
}

string lui(const string& stringBinary){
  string instruction;
  string imm = sub_string(stringBinary, 0, 19);
  string rd  = sub_string(stringBinary, 20, 24);
  string hex_imm = "0x";
  hex_imm += to_string(binary_to_decimal(0, sub_string(stringBinary, 0, 3)));
  hex_imm += to_string(binary_to_decimal(0, sub_string(stringBinary, 4, 7)));
  hex_imm += to_string(binary_to_decimal(0, sub_string(stringBinary, 8, 11)));
  hex_imm += to_string(binary_to_decimal(0, sub_string(stringBinary, 12, 15)));
  hex_imm += to_string(binary_to_decimal(0, sub_string(stringBinary, 16, 19)));
  instruction = "lui x" + to_string(binary_to_decimal(0, rd)) + " " + hex_imm;
  return instruction;
}

// New function to support auipc instruction.
string auipc(const string& stringBinary){
  string instruction;
  // Extract the 20-bit immediate (bits 31:12) and the destination register.
  string imm = sub_string(stringBinary, 0, 19);
  string rd  = sub_string(stringBinary, 20, 24);
  
  // Convert the immediate from binary to decimal, then shift left by 12 bits.
  int imm_val = binary_to_decimal(0, imm);
  int shifted_imm = imm_val << 12; // equivalent to imm_val * 4096
  
  // Format the immediate as a hexadecimal string.
  char buffer[20];
  sprintf(buffer, "0x%x", shifted_imm);
  
  instruction = "auipc x" + to_string(binary_to_decimal(0, rd)) + " " + string(buffer);
  return instruction;
}

string disassembler(int total_instructions, string instruction, int inst_no){
  string disassem_instruction;
  string instruction_binary;
  
  // Convert the hex instruction to a 32-bit binary string.
  for(int i = 0; i < size_of_instruction_hex; i++)
    instruction_binary += convert_to_binary(instruction[i]);
  
  string opcode = sub_string(instruction_binary, 25, 31);
  if(opcode == "0110011") disassem_instruction = R_format(instruction_binary);
  else if(opcode == "0010011") disassem_instruction = I_format_immediate(instruction_binary);
  else if(opcode == "0000011") disassem_instruction = I_format_load(instruction_binary);
  else if(opcode == "0100011") disassem_instruction = S_format(instruction_binary);
  else if(opcode == "1100011") disassem_instruction = B_format(instruction_binary, inst_no, total_instructions);
  else if(opcode == "1101111") disassem_instruction = jal(instruction_binary, inst_no, total_instructions);
  else if(opcode == "1100111") disassem_instruction = jalr(instruction_binary);
  else if(opcode == "0110111") disassem_instruction = lui(instruction_binary);
  else if(opcode == "0010111") disassem_instruction = auipc(instruction_binary);
  else disassem_instruction = "Invalid instruction"; 

  return disassem_instruction;
}

