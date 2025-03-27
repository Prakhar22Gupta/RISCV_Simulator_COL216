#ifndef __parser__
#define __parser__

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

	std::unordered_map<std::string, int> registerMap;
	std::vector<std::vector<std::string>> total_cmds;
	std::vector<int> commandCount;

	Parser(std::ifstream &file, bool forwarding_state)
	{
		for (int i = 0; i < 32; ++i)  registerMap["x" + std::to_string(i)] = i;
		
		struct Bypass* jinks = new Bypass(forwarding_state);
		delay ={50,50,50,50,50};
		bypass_storage = jinks->declare_bypass;
		pipeline = new Pipeline(forwarding_state,{"IF","ID","EX","MEM","WB"}); 
		constructCommands(file);
		commandCount.assign(total_cmds.size(), 0);
	}

    struct Command* define(std::vector<string> command){
        vector<int> hazard_store=bypass_storage[command[0]];
        vector<int> register_values;
        vector<string> type1={"add","sub","and","or","slt","mul","div"},   	  // R type commands
		               type2={"addi","andi","ori","slli","srli","srai"},  // I type commands
					   type3={"beq","bne","bge","blt"}, 				  // B type commands
					   type4={"jal","jalr"};						  // J type commands
		int immediate_val;
		std::vector<std::string> index_of_registers;
		std::copy_if(command.begin(), command.end(), std::back_inserter(index_of_registers),
    	[](const std::string& s) { return s.find("x") != std::string::npos; });

		//for R type commands 3 registers stored
		if(std::find(type1.begin(),type1.end(),command[0])!=type1.end()){
			register_values={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],registerMap[index_of_registers[2]]};
			immediate_val=-1;
		}
		//for I type commands 2 registers and a constant
		else if(std::find(type2.begin(),type2.end(),command[0])!=type2.end()){
			register_values={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],-1};
			immediate_val=stoi(command[3]);
		}
		//for load and store commands
		else if(command[0]=="lw" || command[0]=="lh" || command[0]=="lb"){
			register_values={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],-1};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				immediate_val=stoi(command[2]);
			}
			else{
				immediate_val=0;
			}
		}
		else if(command[0]=="sw" || command[0]=="sh" || command[0]=="sb"){
			register_values={-1,registerMap[index_of_registers[0]],registerMap[index_of_registers[1]]};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				immediate_val=stoi(command[2]);
			}
			else{
				immediate_val=0;
			}
		}
		
		// for branches
		else if(std::find(type3.begin(),type3.end(),command[0])!=type3.end()){
			register_values={-1,registerMap[index_of_registers[0]],registerMap[index_of_registers[1]]};
			immediate_val=-1;
		}
		else if(std::find(type4.begin(),type4.end(),command[0])!=type4.end()){
			// For jalr, use both rd and rs1.
			if(command[0] == "jalr"){
				register_values = { registerMap[index_of_registers[0]], registerMap[index_of_registers[1]], -1 };
				immediate_val = stoi(command[3]);
			}
			else{
				// For j and jal
				register_values = { registerMap[index_of_registers[0]], -1, -1 };
				immediate_val = stoi(command[2]);
			}
		}
		else if (command[0] == "auipc") {
			register_values = { registerMap[index_of_registers[0]], -1, -1 };
			immediate_val = stoi(command[2]);
		}
		else{
			std:: cerr <<"error in command type"<<std::endl;
		}

		vector<string> stage = {"IF","ID","EX","MEM","WB"};
		vector<int> z;
		unordered_map<string, int> stagemap;
		for (int i = 0; i <5; i++) {
            stagemap[stage[i]] = i;
        }
		for(int i=0;i<5;i++){
			z.push_back(stagemap[stage[i]]);
		}
        string instr=command[0];
        struct Command* cmd = new Command(z,hazard_store,register_values,stage,instr,0,immediate_val);
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
		total_cmds.push_back(command);
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