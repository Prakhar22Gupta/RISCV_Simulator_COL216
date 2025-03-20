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

#include "requirements.hpp"
#include "pipeline.hpp"

struct Configuration
{
    vector<vector<int>> parametrs;
    unordered_map<string,vector<int>> indices;
    unordered_map<string,vector<string>> stage_names;
    struct Pipeline *pipeline;

    Configuration(int question) {
        switch (question)
        {
        case 1:
            // 5-stage RISC-V pipeline without branch delay
            stage_names = {
                {"add",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"and",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"or",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"sub",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"mul",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"beq",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"bne",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"slt",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"jal",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"lw",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"sw",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"addi", { "IF", "ID", "EX", "MEM", "WB" }},
                {"slli", { "IF", "ID", "EX", "MEM", "WB" }},
                {"srli", { "IF", "ID", "EX", "MEM", "WB" }}
            };
            parametrs = { {1,20}, {70,100,150,200,120} };
            indices = {
                {"add",  {2,2,2,4}},
                {"and",  {2,2,2,4}},
                {"or",   {2,2,2,4}},
                {"sub",  {2,2,2,4}},
                {"mul",  {2,2,2,4}},
                {"beq",  {2,2,2,-1}},
                {"bne",  {2,2,2,-1}},
                {"slt",  {2,2,2,4}},
                {"jal",  {-1,-1,1,4}}, // now writes back in stage WB (index 4)
                {"lw",   {2,-1,2,4}},
                {"sw",   {2,2,3,-1}},
                {"addi", {2,-1,2,4}},
                {"slli", {2,-1,2,4}},
                {"srli", {2,-1,2,4}}
            };
            pipeline = new Pipeline(5, false, true, 32, 0,
                                    {"IF","ID","EX","MEM","WB"}, false);
            break;
        case 2:
            // Alternative 5-stage configuration (e.g. with different timing)
            stage_names = {
                {"add",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"and",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"or",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"sub",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"mul",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"beq",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"bne",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"slt",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"jal",  { "IF", "ID", "EX", "MEM", "WB" }},
                {"lw",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"sw",   { "IF", "ID", "EX", "MEM", "WB" }},
                {"addi", { "IF", "ID", "EX", "MEM", "WB" }},
                {"slli", { "IF", "ID", "EX", "MEM", "WB" }},
                {"srli", { "IF", "ID", "EX", "MEM", "WB" }}
            };
            parametrs = { {1,20}, {70,100,150,200,120} };
            indices = {
                {"add",  {2,2,2,4}},
                {"and",  {2,2,2,4}},
                {"or",   {2,2,2,4}},
                {"sub",  {2,2,2,4}},
                {"mul",  {2,2,2,4}},
                {"beq",  {2,2,2,-1}},
                {"bne",  {2,2,2,-1}},
                {"slt",  {2,2,2,4}},
                {"jal",  {-1,-1,1,4}},
                {"lw",   {2,-1,2,4}},
                {"sw",   {3,2,3,-1}},
                {"addi", {2,-1,2,4}},
                {"slli", {2,-1,2,4}},
                {"srli", {2,-1,2,4}}
            };
            pipeline = new Pipeline(5, false, true, 32, 0,
                                    {"IF","ID","EX","MEM","WB"}, false);
            break;

        default:
            std::cerr << "Invalid question number" << std::endl;
            break;
        }
    }
};

#endif
