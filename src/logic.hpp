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

    int numberofstages;
    bool firstinfirstoutactive;
    bool bypassactive;
    bool symmetryactive;
    vector<int> stageemptytime;
    vector<int> pseudostageemptytime;
    vector<int> stagestarttime;
    vector<int> pseudostagestarttime;
    int numberofregisters;
    struct Registerfile* registerfile;
    struct Registerfile* pseudoregisterfile;
    vector<Runtimedata*> pseudoruntimelist;
    vector<Runtimedata*> history;
    int starttime;
    int timetaken;
    unordered_map<string, int> stagemap;
    int cycle;
 
    Pipeline(int param1, bool param2, bool param3, int param4, int param5, vector<string> param6,bool param7) {
        numberofstages = param1;
        firstinfirstoutactive= param7;
        bypassactive = param2;
        symmetryactive = param3;
        numberofregisters = param4;
        registerfile = new Registerfile(param4);
        pseudoregisterfile = new Registerfile(param4);
        stageemptytime.resize(param1);
        vector<int> temp1(param1,0);
        stageemptytime = temp1;
        pseudostageemptytime.resize(param1);
        vector<int> temp2(param1,0);
        pseudostageemptytime = temp2;
        stagestarttime.resize(param1);
        vector<int> temp3(param1,0);
        stagestarttime = temp3;
        pseudostagestarttime.resize(param1);
        vector<int> temp4(param1,0);
        pseudostagestarttime = temp4;
        starttime = param5;
        timetaken = 0;
        for (int i = 0; i < (int)param6.size(); i++) {
            stagemap[param6[i]] = i;
        }
        history.resize(0);
        pseudoruntimelist.resize(0);
        cycle=-1;
    }

    struct Runtimedata* run_command(Command* param1){

    vector<int> v = {param1->destinationregister,param1->sourceregister1,param1->sourceregister2};
    vector<int> v1 = {param1->bypassindex1,param1->bypassindex2,param1->readindex,param1->writeindex};
    struct Command* command = new Command(false,param1->intermediatelatchlength,param1->stagelengths,v1,v,param1->stagenames,param1->opcode,param1->value,param1->constant);
    
    if (param1->intermediatelatchesactive){
        for(int i=0;i<(int)command->stagelengths.size()-1;i++){
            command->stagelengths[i]+=command->intermediatelatchlength;
        }
    }

    if (symmetryactive){
        vector<int> temp1(command->numberofstages,*max_element(command->stagelengths.begin(),command->stagelengths.end()));
        command->stagelengths= temp1;
        if(cycle==-1){
            cycle=temp1[0];
        }
        else{
            if(cycle!=temp1[0]){
                cycle=-2;
            }
        }
    }

    bool firstinfirstouttemp=firstinfirstoutactive;
    if(command->destinationregister!=-1){
        firstinfirstouttemp=false;
    }
    struct Runtimedata* runtime = new Runtimedata(command,stageemptytime[0],command->numberofstages);
    runtime->stagenames[0]=command->stagenames[0];
    runtime->stages[0]={pseudostageemptytime[stagemap[command->stagenames[0]]],-1};
    pseudostagestarttime[stagemap[command->stagenames[0]]]=pseudostageemptytime[stagemap[command->stagenames[0]]];

    for (int j = 0; j < command->numberofstages - 1; j++) {
        int endtime = runtime->stages[j][0] + command->stagelengths[j];

        //Hazard check at ID stage
        if ((j + 1) == command->readindex) {
            if(!bypassactive){
                if (command->sourceregister1 != -1) {
                    endtime = max(endtime, pseudoregisterfile->updatetime[command->sourceregister1]);
                }
                if (command->sourceregister2 != -1) {
                    endtime = max(endtime, pseudoregisterfile->updatetime[command->sourceregister2]);
                }
            }
            else{
                if (command->sourceregister1 != -1) {
                    endtime = max(endtime, pseudoregisterfile->intermediateupdatetime[command->sourceregister1]);
                }
                if (command->sourceregister2 != -1) {
                    endtime = max(endtime, pseudoregisterfile->intermediateupdatetime[command->sourceregister2]);
                }
            }

        }
        // (You may keep any additional stage-boundary constraints below.)
        if (endtime < pseudostageemptytime[stagemap[command->stagenames[j + 1]]]
            && (endtime > pseudostagestarttime[stagemap[command->stagenames[j + 1]]] || firstinfirstouttemp)) {
            endtime = pseudostageemptytime[stagemap[command->stagenames[j + 1]]];
        }

        // Update register file times when applicable
        if (command->destinationregister != -1) {

            //This is for EX stage in the case of forwarding
            if (j == command->readindex) {
                // Update intermediate updatetime at read stage.
                pseudoregisterfile->intermediateupdatetime[command->destinationregister] = endtime;
            }
            if (j == command->writeindex) {
                // At the write-back stage, update the register's updatetime.
                pseudoregisterfile->updatetime[command->destinationregister] = endtime;
                pseudoregisterfile->queue->enqueue(command->destinationregister, command->value, endtime);
            }
        }
        runtime->stages[j][1] = endtime;
        runtime->stagenames[j + 1] = command->stagenames[j + 1];
        runtime->stages[j + 1] = { endtime, -1 };
        pseudostageemptytime[stagemap[command->stagenames[j]]] = endtime;
        pseudostagestarttime[stagemap[command->stagenames[j + 1]]] = endtime;
    }


    pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]] =
        runtime->stages[command->numberofstages - 1][0] + command->stagelengths[command->numberofstages - 1];
    if (pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]] > timetaken) {
        timetaken = pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]];
    }
    if (command->destinationregister != -1) {
        if (command->writeindex == command->numberofstages - 1) {
            pseudoregisterfile->updatetime[command->destinationregister] =
                pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]];
            pseudoregisterfile->queue->enqueue(command->destinationregister, command->value,
                pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]]);
        }
        if (command->readindex == command->numberofstages - 1) {
            pseudoregisterfile->intermediateupdatetime[command->destinationregister] =
                pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]];
        }
        registerfile->values[command->destinationregister] = command->value;
    }
    runtime->stages[command->numberofstages - 1][1] =
    pseudostageemptytime[stagemap[command->stagenames[command->numberofstages - 1]]];
    registerfile->values[command->destinationregister] = command->value;
    pseudoruntimelist.push_back(runtime);
    return runtime;
}

    void save() {
    history.insert(history.end(), pseudoruntimelist.begin(), pseudoruntimelist.end());
    if(pseudoruntimelist.size() > 0){
        pseudoruntimelist.clear();
    }
    registerfile = pseudoregisterfile->copy_file();
    stageemptytime = pseudoregisterfile->copy_vector(pseudostageemptytime);
    }

    void restore() {
        if(pseudoruntimelist.size() > 0){
            pseudoruntimelist.clear();
        }
        pseudoregisterfile = registerfile->copy_file();
        pseudostageemptytime = registerfile->copy_vector(stageemptytime);
    }

    void insert_halt(Command* com) {
        stageemptytime[0] = history[(int)history.size()-1]->stages[com->readindex][1];
        pseudostageemptytime[0] = history[(int)history.size()-1]->stages[com->readindex][1];
        if(pseudoruntimelist.size() > 0){
            pseudoruntimelist.clear();
        }
        pseudoregisterfile = registerfile->copy_file();
        pseudostageemptytime = registerfile->copy_vector(stageemptytime);
    }

    void print_pipeline() {
        for (int i = 0; i < (int)history.size(); i++) {
            history[i]->print_runtime();
        }
    }

    void print_table(){
        if(cycle<0){
            cerr << "Error: Pipeline is too asymetric to print a table" << endl;
        }
        int numcycles=timetaken/cycle;
        cout<<"   ||";
        for (int i = 0; i < numcycles; i++) {
            cout<<i<<"";
            if(i<10){
                cout<<"   ";
            }
            else if(i<100){
                cout<<"  ";
            }
            else if(i<1000){
                cout<<" ";
            }
            cout<<"|";
        }
        cout<<endl;
        for(int i=0; i<(int)history.size(); i++){
            cout<<i<<"";
            if(i<10){
                cout<<"  ";
            }
            else if(i<100){
                cout<<" ";
            }
            cout<<"||";
            int x= history[i]->stages[0][0];
            int j=x/cycle;
            for(;j>0;j--){
                std::cout<<"    |";
            }
            for(int k=0; k<(int)history[i]->stages.size(); k++){
                string name=history[i]->stagenames[k];
                if (name.length() >= 4) {
                    name=name.substr(0, 4);
                } else {
                    name.append(4 - name.length(), ' ');
                }
                std::cout<<name<<"|";
                int y= history[i]->stages[k][1]-history[i]->stages[k][0]-cycle;
                int l=y/cycle;
                for(;l>0;l--){
                    std::cout<<"  * |";
                }
            }
            int z=timetaken-history[i]->stages[(int)history[i]->stages.size()-1][1];
            int m=z/cycle;
            for(;m>0;m--){
                std::cout<<"    |";
            }
            std::cout<<""<<endl;
        }
    }
};

#endif