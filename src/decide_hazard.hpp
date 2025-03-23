#ifndef __CONFIGURATION_HPP__
#define __CONFIGURATION_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

#include "register_file.hpp"
#include "logic.hpp"

struct Configuration
{
    vector<vector<int>> parametrs;
    unordered_map<string,vector<int>> indices;
    unordered_map<string,vector<string>> stage_names;
    struct Pipeline *pipeline;

    Configuration(){
            stage_names = {
                {"add",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"and",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"or",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"sub",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"mul",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"slt",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"lw",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"sw",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"addi", { "IF", "ID", "EX", "MEM", "WB" }},
                {"slli", { "IF", "ID", "EX", "MEM", "WB" }},
                {"srli", { "IF", "ID", "EX", "MEM", "WB" }},
                // {"beq",  { "IF", "ID", "EX", "MEM", "WB" }},
                // {"bne",  { "IF", "ID", "EX", "MEM", "WB" }},
                // {"jal",  { "IF", "ID", "EX", "MEM", "WB" }},
                // {"jalr", { "IF", "ID", "EX", "MEM", "WB" }}
            };
            parametrs = { {1,20}, {70,100,150,200,120} };
            indices = {
                {"add",  {2,2,2,4}},
                {"and",  {2,2,2,4}},
                {"or",   {2,2,2,4}},
                {"sub",  {2,2,2,4}},
                {"mul",  {2,2,2,4}},
                {"slt",  {2,2,2,4}},
                {"lw",   {2,-1,2,4}},
                {"sw",   {2,2,2,-1}},
                {"addi", {2,-1,2,4}},
                {"slli", {2,-1,2,4}},
                {"srli", {2,-1,2,4}},
                // {"beq",  {2,2,2,-1}},
                // {"bne",  {2,2,2,-1}},
                // {"jal",  {-1,-1,1,4}}, 
                // {"jalr", {1,-1,1,4}}
            };
            pipeline = new Pipeline(5,false, true, 32, 0,
                                    {"IF","ID","EX","MEM","WB"}, false); 
    }
};

#endif
