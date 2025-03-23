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

    vector<vector<int>> parametrs;
    std:: unordered_map<string,vector<int>> indices;
    std:: unordered_map<string,vector<string>> stage_names;
	struct Pipeline *pipeline;

	std::unordered_map<std::string, int> registerMap, address;
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;

	Parser(std::ifstream &file)
	{
		for (int i = 0; i < 32; ++i)  registerMap["x" + std::to_string(i)] = i;
			
			registerMap["zero"] = 0;
			registerMap["ra"] = 1;
			registerMap["sp"] = 2;
			registerMap["gp"] = 3;
			registerMap["tp"] = 4;
			registerMap["t0"] = 5;
			registerMap["t1"] = 6;
			registerMap["t2"] = 7;
			registerMap["s0"] = 8;
			registerMap["fp"] = 8;
			registerMap["s1"] = 9;
			for (int i = 0; i < 8; ++i)
    			registerMap["a" + std::to_string(i)] = 10 + i;
			for (int i = 0; i < 10; ++i)
				registerMap["s" + std::to_string(i+2)] = 18 + i;
			for (int i = 0; i < 4; ++i)
				registerMap["t" + std::to_string(i+3)] = 28 + i;
		
		struct Configuration* config = new Configuration();
		parametrs = config->parametrs;
		indices = config->indices;
		stage_names = config->stage_names;
		pipeline = config->pipeline;
		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

    struct Command* define(std::vector<string> command){
        bool param1=parametrs[0][0]==1;
        int param2=parametrs[0][1];
        vector<int> param4=indices[command[0]];
        vector<int> param5;
        vector<string> command_type1={"add","sub","and","or","slt","mul"}, command_type2={"addi","andi","ori","sll","srl"};
		vector<string> command_type3={"beq","bne"}, command_type4={"j","jal","jalr"};
		int param9;
		std::vector<std::string> subsetVec;
		std::copy_if(command.begin(), command.end(), std::back_inserter(subsetVec),
    	[](const std::string& s) { return s.find("x") != std::string::npos; });

		//for R type commands 3 registers stored
		if(std::find(command_type1.begin(),command_type1.end(),command[0])!=command_type1.end()){
			param5={registerMap[subsetVec[0]],registerMap[subsetVec[1]],registerMap[subsetVec[2]]};
			param9=-1;
		}
		//for I type commands 2 registers and a constant
		else if(std::find(command_type2.begin(),command_type2.end(),command[0])!=command_type2.end()){
			param5={registerMap[subsetVec[0]],registerMap[subsetVec[1]],-1};
			param9=stoi(command[3]);
		}
		else if(command[0]=="lw"){
			param5={registerMap[subsetVec[0]],registerMap[subsetVec[1]],-1};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				param9=stoi(command[2]);
			}
			else{
				param9=0;
			}
		}
		else if(command[0]=="sw"){
			param5={-1,registerMap[subsetVec[0]],registerMap[subsetVec[1]]};
			if(std::find(command[2].begin(),command[2].end(),'x')==command[2].end()){
				param9=stoi(command[2]);
			}
			else{
				param9=0;
			}
		}
		else if(std::find(command_type3.begin(),command_type3.end(),command[0])!=command_type3.end()){
			param5={-1,registerMap[subsetVec[0]],registerMap[subsetVec[1]]};
			param9=-1;
		}
		else if(std::find(command_type4.begin(),command_type4.end(),command[0])!=command_type4.end()){
			param5 = { registerMap[subsetVec[0]], -1, -1 };
			param9 = stoi(command[2]);
		}
		else{
			std:: cerr <<"error in command type"<<std::endl;
		}

		
        vector<string> param6=stage_names[command[0]];
		vector<int> param3;
		unordered_map<string, int> stagemap;
		for (int i = 0; i < (int)param6.size(); i++) {
            stagemap[param6[i]] = i;
        }
		for(int i=0;i<param6.size();i++){
			param3.push_back(parametrs[1][stagemap[param6[i]]]);
		}
        string param7=command[0];
        struct Command* cmd = new Command(param1,param2,param3,param4,param5,param6,param7,0,param9);
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
		command.resize(4);
		commands.push_back(command);
        struct Command* parametric= define(command);
        parametric_commands.push_back(parametric);
	}

	void print_commands()
	{
		for(int i=0;i<(int)commands.size();i++){
			parametric_commands[i]->print_command();
		}
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
