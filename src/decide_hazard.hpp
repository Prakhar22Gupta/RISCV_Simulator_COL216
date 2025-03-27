#ifndef __DECIDE_HAZARD_HPP__
#define __DECIDE_HAZARD_HPP__

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
        Bypass(bool forwarding_state){
            if(forwarding_state){
                declare_bypass = {
                    {"add",  {2,2,2,4}},
                    {"and",  {2,2,2,4}},
                    {"or",   {2,2,2,4}},
                    {"sub",  {2,2,2,4}},
                    {"mul",  {2,2,2,4}},
                    {"div",  {2,2,2,4}},
                    {"rem",  {2,2,2,4}},
                    {"addi", {2,-1,2,4}},
                    {"andi", {2,-1,2,4}},
                    {"ori",  {2,-1,2,4}},
                    {"slli", {2,-1,2,4}},
                    {"srli", {2,-1,2,4}},
                    {"srai", {2,-1,2,4}},
                    {"beq",  {1,1,2,-1}},
                    {"bge",  {1,1,2,-1}},
                    {"blt",  {1,1,2,-1}},
                    {"bne",  {1,1,2,-1}},
                    {"slt",  {1,1,2,4}},
                    {"lw",   {3,-1,2,4}},
                    {"sw",   {2,-1,2,-1}},
                    {"lb",   {3,-1,2,4}},
                    {"sb",   {2,-1,2,-1}},
                    {"lh",   {3,-1,2,4}},
                    {"sh",   {2,-1,2,-1}},
                    {"jal",  {-1,-1,2,4}}, 
                    {"jalr", {1,-1,2,4}},
                    {"auipc", {-1,-1,-1,4}}
                };
            }
            else
            declare_bypass = {
                {"add",  {2,2,2,4}},
                {"and",  {2,2,2,4}},
                {"or",   {2,2,2,4}},
                {"sub",  {2,2,2,4}},
                {"mul",  {2,2,2,4}},
                {"div",  {2,2,2,4}},
                {"rem",  {2,2,2,4}},
                {"addi", {2,-1,2,4}},
                {"andi", {2,-1,2,4}},
                {"ori",  {2,-1,2,4}},
                {"slli", {2,-1,2,4}},
                {"srli", {2,-1,2,4}},
                {"srai", {2,-1,2,4}},
                {"beq",  {2,2,2,-1}},
                {"bge",  {2,2,2,-1}},
                {"blt",  {2,2,2,-1}},
                {"bne",  {2,2,2,-1}},
                {"slt",  {2,2,2,4}},
                {"lw",   {3,-1,2,4}},
                {"sw",   {2,-1,2,-1}},
                {"lb",   {3,-1,2,4}},
                {"sb",   {2,-1,2,-1}},
                {"lh",   {3,-1,2,4}},
                {"sh",   {2,-1,2,-1}},
                {"jal",  {-1,-1,2,4}}, 
                {"jalr", {2,-1,2,4}},
                {"auipc", {-1,-1,-1,4}}
            };
    }
};

#endif
