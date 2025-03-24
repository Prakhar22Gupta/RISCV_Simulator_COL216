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

	std::unordered_map<std::string, int> registerMap, address;
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;

	Parser(std::ifstream &file)
	{
		for (int i = 0; i < 32; ++i)  registerMap["x" + std::to_string(i)] = i;
		
		struct Bypass* jinks = new Bypass();
		delay ={50,50,50,50,50};
		bypass_storage = jinks->declare_bypass;
		pipeline = new Pipeline(false, true, 32, 0,{"IF","ID","EX","MEM","WB"}, false); 
		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

    struct Command* define(std::vector<string> command){
        vector<int> param4=bypass_storage[command[0]];
        vector<int> param5;
        vector<string> command_type1={"add","sub","and","or","slt","mul"},   	  // R type commands
		               command_type2={"addi","andi","ori","slli","srli","srai"},  // I type commands
					   command_type3={"beq","bne","bge","blt"}, 				  // B type commands
					   command_type4={"j","jal","jalr"};						  // J type commands
		int param9;
		std::vector<std::string> index_of_registers;
		std::copy_if(command.begin(), command.end(), std::back_inserter(index_of_registers),
    	[](const std::string& s) { return s.find("x") != std::string::npos; });

		//for R type commands 3 registers stored
		if(std::find(command_type1.begin(),command_type1.end(),command[0])!=command_type1.end()){
			param5={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],registerMap[index_of_registers[2]]};
			param9=-1;
		}
		//for I type commands 2 registers and a constant
		else if(std::find(command_type2.begin(),command_type2.end(),command[0])!=command_type2.end()){
			param5={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],-1};
			param9=stoi(command[3]);
		}
		//for load and store commands
		else if(command[0]=="lw"){
			param5={registerMap[index_of_registers[0]],registerMap[index_of_registers[1]],-1};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				param9=stoi(command[2]);
			}
			else{
				param9=0;
			}
		}
		else if(command[0]=="sw"){
			param5={-1,registerMap[index_of_registers[0]],registerMap[index_of_registers[1]]};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				param9=stoi(command[2]);
			}
			else{
				param9=0;
			}
		}
		else if(command[0] == "lb"){
			param5 = { registerMap[index_of_registers[0]], registerMap[index_of_registers[1]], -1 };
			if(std::find(command[2].begin(), command[2].end(),'x') == command[2].end()){
				param9 = stoi(command[2]);
			}
			else{
				param9 = 0;
			}
		}
		else if(command[0]=="sb"){
			param5 = { -1, registerMap[index_of_registers[0]], registerMap[index_of_registers[1]] };
			if(std::find(command[2].begin(), command[2].end(), 'x') == command[2].end()){
				param9 = stoi(command[2]);
			}
			else{
				param9 = 0;
			}
		}
		else if(command[0]=="ld"){
			param5 = { registerMap[index_of_registers[0]], registerMap[index_of_registers[1]], -1 };
			if(std::find(command[2].begin(), command[2].end(), 'x') == command[2].end()){
				param9 = stoi(command[2]);
			}
			else{
				param9 = 0;
			}
		}
		else if(command[0]=="sd"){
			param5 = { -1, registerMap[index_of_registers[0]], registerMap[index_of_registers[1]] };
			if(std::find(command[2].begin(), command[2].end(), 'x') == command[2].end()){
				param9 = stoi(command[2]);
			}
			else{
				param9 = 0;
			}
		}
		
		// for branches
		else if(std::find(command_type3.begin(),command_type3.end(),command[0])!=command_type3.end()){
			param5={-1,registerMap[index_of_registers[0]],registerMap[index_of_registers[1]]};
			param9=-1;
		}
		else if(std::find(command_type4.begin(),command_type4.end(),command[0])!=command_type4.end()){
			// For jalr, use both rd and rs1.
			if(command[0] == "jalr"){
				param5 = { registerMap[index_of_registers[0]], registerMap[index_of_registers[1]], -1 };
				param9 = stoi(command[3]);
			}
			else{
				// For j and jal
				param5 = { registerMap[index_of_registers[0]], -1, -1 };
				param9 = stoi(command[2]);
			}
		}
		else{
			std:: cerr <<"error in command type"<<std::endl;
		}

		vector<string> param6 = {"IF","ID","EX","MEM","WB"};
		vector<int> param3;
		unordered_map<string, int> stagemap;
		for (int i = 0; i < (int)param6.size(); i++) {
            stagemap[param6[i]] = i;
        }
		for(int i=0;i<param6.size();i++){
			param3.push_back(stagemap[param6[i]]);
		}
        string param7=command[0];
        struct Command* cmd = new Command(param3,param4,param5,param6,param7,0,param9);
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
		// empty line or a comment only line
		if (command.empty()){
			return;
		}
		// else if (command.size() == 1)
		// {
		// 	std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
		// 	if (address.find(label) == address.end()){
		// 		address[label] = commands.size();
		// 	}
		// 	else{
		// 		address[label] = -1;
		// 	}
		// 	command.clear();
		// }
		// else if (command[0].back() == ':')
		// {
		// 	std::string label = command[0].substr(0, command[0].size() - 1);
		// 	if (address.find(label) == address.end()){
		// 		address[label] = commands.size();
		// 	}
		// 	else{
		// 		address[label] = -1;
		// 	}
		// 	command = std::vector<std::string>(command.begin() + 1, command.end());
		// }
		// else if (command[0].find(':') != std::string::npos)
		// {
		// 	int idx = command[0].find(':');
		// 	std::string label = command[0].substr(0, idx);
		// 	if (address.find(label) == address.end()){
		// 		address[label] = commands.size();
		// 	}
		// 	else{
		// 		address[label] = -1;
		// 	}
		// 	command[0] = command[0].substr(idx + 1);
		// }
		// else if (command[1][0] == ':')
		// {
		// 	if (address.find(command[0]) == address.end()){
		// 		address[command[0]] = commands.size();
		// 	}
		// 	else{
		// 		address[command[0]] = -1;
		// 	}
		// 	command[1] = command[1].substr(1);
		// 	if (command[1] == ""){
		// 		command.erase(command.begin(), command.begin() + 2);
		// 	}
		// 	else{
		// 		command.erase(command.begin(), command.begin() + 1);
		// 	}
		// }
		// if (command.empty()){
		// 	return;
		// }
		// if (command.size() > 4){
		// 	for (int i = 4; i < (int)command.size(); ++i){
		// 		command[3] += " " + command[i];
		// 	}
		// }
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
