#ifndef _DECIDE_HAZARD_HPP_
#define _DECIDE_HAZARD_HPP_

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

struct Bypass
{
    unordered_map<string,vector<int>> declare_bypass;
        Bypass(){
            declare_bypass = {
                {"add",  {2,2,2,4}},
                {"and",  {2,2,2,4}},
                {"or",   {2,2,2,4}},
                {"sub",  {2,2,2,4}},
                {"mul",  {2,2,2,4}},
                {"slt",  {2,2,2,4}},
                {"lw",   {3,-1,2,4}},
                {"sw",   {2,-1,2,-1}},
                {"lb",   {3,-1,2,4}},
                {"sb",   {2,-1,2,-1}},
                {"addi", {2,-1,2,4}},
                {"slli", {2,-1,2,4}},
                {"srli", {2,-1,2,4}},
                {"srai", {2,-1,2,4}},
                {"beq",  {2,2,2,-1}},
                {"bge",  {2,2,2,-1}},
                {"blt",  {2,2,2,-1}},
                {"bne",  {2,2,2,-1}},
                {"jal",  {-1,-1,2,4}}, 
                {"jalr", {2,-1,2,4}}
            };
    }
};

#endif
