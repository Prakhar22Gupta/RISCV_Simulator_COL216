#ifndef __PARSER__
#define __PARSER__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>
#include "decide_hazard.hpp"
#include "logic.hpp"
#include "register_file.hpp"

using namespace std;

struct Parser
{
    vector< struct Command* > parametric_commands;

    vector<int> delay;
    std:: unordered_map<string,vector<int>> bypass_storage;
	struct Pipeline *pipeline;

	std::unordered_map<std::string, int> registerMap, address;
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;

	Parser(std::ifstream &file, bool forwarding_state)
	{
		for (int i = 0; i < 32; ++i)  registerMap["x" + std::to_string(i)] = i;
		
		struct Bypass* jinks = new Bypass(forwarding_state);
		delay ={50,50,50,50,50};
		bypass_storage = jinks->declare_bypass;
		pipeline = new Pipeline(forwarding_state, true,{"IF","ID","EX","MEM","WB"}, false); 
		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

    struct Command* define(std::vector<string> command){
        vector<int> hazard_store=bypass_storage[command[0]];
        vector<int> reg_values;
        vector<string> command_type1={"add","sub","and","or","slt","mul","div","rem"},   	  // R type commands
		               command_type2={"addi","andi","ori","slli","srli","srai"},  // I type commands
					   command_type3={"beq","bne","bge","blt"}, 				  // B type commands
					   command_type4={"jal","jalr"};						  // J type commands
		int imm_val;
		std::vector<std::string> index_of_registers;
		std::copy_if(command.begin(), command.end(), std::back_inserter(index_of_registers),
    	[](const std::string& s) { return s.find("x") != std::string::npos; });

		//for R type commands 3 registers stored
		if(std::find(command_type1.begin(),command_type1.end(),command[0])!=command_type1.end()){
			reg_values={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],registerMap[index_of_registers[2]]};
			imm_val=-1;
		}
		//for I type commands 2 registers and a constant
		else if(std::find(command_type2.begin(),command_type2.end(),command[0])!=command_type2.end()){
			reg_values={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],-1};
			imm_val=stoi(command[3]);
		}
		//for load and store commands
		else if(command[0]=="lw" || command[0]=="lh" || command[0]=="lb"){
			reg_values={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],-1};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				imm_val=stoi(command[2]);
			}
			else{
				imm_val=0;
			}
		}
		else if(command[0]=="sw" || command[0]=="sh" || command[0]=="sb"){
			reg_values={-1,registerMap[index_of_registers[0]],registerMap[index_of_registers[1]]};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				imm_val=stoi(command[2]);
			}
			else{
				imm_val=0;
			}
		}
		
		// for branches
		else if(std::find(command_type3.begin(),command_type3.end(),command[0])!=command_type3.end()){
			reg_values={-1,registerMap[index_of_registers[0]],registerMap[index_of_registers[1]]};
			imm_val=-1;
		}
		else if(std::find(command_type4.begin(),command_type4.end(),command[0])!=command_type4.end()){
			// For jalr, use both rd and rs1.
			if(command[0] == "jalr"){
				reg_values = { registerMap[index_of_registers[0]], registerMap[index_of_registers[1]], -1 };
				imm_val = stoi(command[3]);
			}
			else{
				// For j and jal
				reg_values = { registerMap[index_of_registers[0]], -1, -1 };
				imm_val = stoi(command[2]);
			}
		}
		else if (command[0] == "auipc") {
			reg_values = { registerMap[index_of_registers[0]], -1, -1 };
			imm_val = stoi(command[2]);
		}
		else{
			std:: cerr <<"error in command type"<<std::endl;
		}

		vector<string> param2 = {"IF","ID","EX","MEM","WB"};
		vector<int> param1;
		unordered_map<string, int> stagemap;
		for (int i = 0; i < (int)param2.size(); i++) {
            stagemap[param2[i]] = i;
        }
		for(int i=0;i<param2.size();i++){
			param1.push_back(stagemap[param2[i]]);
		}
        string param3=command[0];
        struct Command* cmd = new Command(param1,hazard_store,reg_values,param2,param3,0,imm_val);
        return cmd;
    }

	void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t()"));
		for (auto &s : tokens){
			command.push_back(s);
		}
	
		if (command.empty()){
			return;
		}
		
		command.resize(4);
		commands.push_back(command);
        struct Command* parametric= define(command);
        parametric_commands.push_back(parametric);
	}

	void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	}
};

#endif