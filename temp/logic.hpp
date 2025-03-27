#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include <stdlib.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "register_file.hpp"

using namespace std;
struct Pipeline{

    bool bypassactive;
    vector<int> stagestarttime;
    vector<int> stageemptytime;
    vector<int> smart_stagestarttime;
    vector<int> smart_stageemptytime;
    struct Registerfile* registerfile;
    struct Registerfile* one_instruction_registerfile;
    vector<Runtimedata*> smart_runtime_list;
    vector<Runtimedata*> history;
    int starttime;
    int timetaken;
    unordered_map<string, int> stagemap;

    Pipeline(bool param1, vector<string> param_0) {
        bypassactive = param1;

        registerfile = new Registerfile(32);
        one_instruction_registerfile = new Registerfile(32);

        stagestarttime.resize(5);
        vector<int> temp3(5,0);
        stagestarttime = temp3;
        smart_stagestarttime.resize(5);
        vector<int> temp4(5,0);
        smart_stagestarttime = temp4;

        stageemptytime.resize(5);
        vector<int> temp1(5,0);
        stageemptytime = temp1;
        smart_stageemptytime.resize(5);
        vector<int> temp2(5,0);
        smart_stageemptytime = temp2;

        starttime = 0;
        timetaken = 0;

        for (int i = 0; i < 5; i++) {
            stagemap[param_0[i]] = i;
        }
        
        history.resize(0);
        smart_runtime_list.resize(0);
    }

    struct Runtimedata* run_command(Command* param1){

    vector<int> v = {param1->destinationregister,param1->sourceregister1,param1->sourceregister2};
    vector<int> v1 = {param1->bypassindex1,param1->bypassindex2,param1->readindex,param1->writeindex};
    struct Command* command = new Command(param1->stagelengths,v1,v,param1->stagenames,param1->opcode,param1->value,param1->constant);
    
    
    vector<int> temp1(5,50);
    command->stagelengths= temp1;
    
    struct Runtimedata* runtime = new Runtimedata(command,stageemptytime[0]);

    runtime->stagenames[0]=command->stagenames[0];                                     
    runtime->stages[0]={smart_stageemptytime[stagemap[command->stagenames[0]]],-1};    //Initialise the start time of the IF stage 
    smart_stagestarttime[stagemap[command->stagenames[0]]]=smart_stageemptytime[stagemap[command->stagenames[0]]];  //Initilaise the start and end time for smart 


    //IF->ID->EX->MEM
    for (int j = 0; j < 4; j++) {
        int endtime = runtime->stages[j][0] + command->stagelengths[j];    //start_time + duration of stage

        //Hazard check at readindex stage
        if ((j + 1) == command->readindex) {
            if(!bypassactive){
                if (command->sourceregister1 != -1) {
                    endtime = max(endtime, one_instruction_registerfile->updatetime[command->sourceregister1]);
                }
                if (command->sourceregister2 != -1) {
                    endtime = max(endtime, one_instruction_registerfile->updatetime[command->sourceregister2]);
                }
            }
            else{
                if (command->sourceregister1 != -1) {
                    endtime = max(endtime, one_instruction_registerfile->forwarding_updatetime[command->sourceregister1]);
                }
                if (command->sourceregister2 != -1) {
                    endtime = max(endtime, one_instruction_registerfile->forwarding_updatetime[command->sourceregister2]);
                }
            }
        }

        // (Keeping additional stage-boundary constraints)
        if (endtime < smart_stageemptytime[stagemap[command->stagenames[j + 1]]]
            && (endtime > smart_stagestarttime[stagemap[command->stagenames[j + 1]]])) {
            endtime = smart_stageemptytime[stagemap[command->stagenames[j + 1]]];
        }

        // Update register file times for both normal and smart
        if (command->destinationregister != -1) {
            //This is for data_forwarding stage in the case of forwarding
            if (j == command->bypassindex1){
                // Update forwarding_updatetime as needed
                one_instruction_registerfile->forwarding_updatetime[command->destinationregister] = endtime;
            }
        }
        runtime->stages[j][1] = endtime;
        // Initialise values for next stage
        runtime->stagenames[j + 1] = command->stagenames[j + 1];
        runtime->stages[j + 1] = { endtime, -1 };
        smart_stageemptytime[stagemap[command->stagenames[j]]] = endtime;
        smart_stagestarttime[stagemap[command->stagenames[j + 1]]] = endtime;
    }

    //for last WB stage
    smart_stageemptytime[stagemap[command->stagenames[4]]] =runtime->stages[4][0] + command->stagelengths[4];
    if (smart_stageemptytime[stagemap[command->stagenames[4]]] > timetaken) {
        timetaken = smart_stageemptytime[stagemap[command->stagenames[4]]];
    }
    if (command->destinationregister != -1) {
        if (command->writeindex == 4) {
            one_instruction_registerfile->updatetime[command->destinationregister] =
                smart_stageemptytime[stagemap[command->stagenames[4]]];
        }
        
        //update the value in global register data structure.
        registerfile->values[command->destinationregister] = command->value;
    }
    runtime->stages[4][1] = smart_stageemptytime[stagemap[command->stagenames[4]]];
    registerfile->values[command->destinationregister] = command->value;
    smart_runtime_list.push_back(runtime);
    return runtime;
}

    void save() {
    history.insert(history.end(), smart_runtime_list.begin(), smart_runtime_list.end());
    if(smart_runtime_list.size() > 0){
        smart_runtime_list.clear();
    }

    //update the global register file
    registerfile = one_instruction_registerfile->copy_file();
    stageemptytime = one_instruction_registerfile->copy_vector(smart_stageemptytime);
    }

   
    void insert_halt(Command* com) {
        stageemptytime[0] = history[(int)history.size()-1]->stages[(com->readindex)-1][1];
        smart_stageemptytime[0] = history[(int)history.size()-1]->stages[(com->readindex)-1][1];
        if(smart_runtime_list.size() > 0){
            smart_runtime_list.clear();
        }
        one_instruction_registerfile = registerfile->copy_file();
        smart_stageemptytime = registerfile->copy_vector(stageemptytime);
    }
    
    void print_table(std::ostream &out = std::cout,int numcycles=0) {
       
        out << "   ||";
        for (int i = 0; i < numcycles; i++) {
            out << i+1;
            if(i+1 < 10)
                out << "   ";
            else if(i+1 < 100)
                out << "  ";
            else if(i+1 < 1000)
                out << " ";
            out << "|";
        }
        out << std::endl;
        for(int i = 0; i < (int)history.size(); i++) {
            out << i+1;
            if(i+1 < 10)
                out << "  ";
            else if(i+1 < 100)
                out << " ";
            out << "||";
            int x = history[i]->stages[0][0];
            int j = x / numcycles;
            for(; j > 0; j--) {
                out << "    |";
            }
            for (int k = 0; k < (int)history[i]->stages.size(); k++) {
                std::string name = history[i]->stagenames[k];
                if (name.length() >= 4) {
                    name = name.substr(0, 4);
                } else {
                    name.append(4 - name.length(), ' ');
                }
                out << name << "|";
                int y = history[i]->stages[k][1] - history[i]->stages[k][0] - numcycles;
                int l = y / numcycles;
                for(; l > 0; l--) {
                    out << "  * |";
                }
            }
            int z = timetaken - history[i]->stages[(int)history[i]->stages.size()-1][1];
            int m = z / numcycles;
            for(; m > 0; m--) {
                out << "    |";
            }
            out << "" << std::endl;
        }
    }
    
};

#endif