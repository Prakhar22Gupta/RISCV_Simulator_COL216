#ifndef __REGISTER_FILE_H
#define __REGISTER_FILE_H
#include <stdlib.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

struct Command{

    public:
    vector<int> stagelengths;
    int bypassindex1;
    int bypassindex2;
    int readindex;
    int writeindex;
    int destinationregister;
    int sourceregister1;
    int sourceregister2;
    vector<string> stagenames;
    string opcode;
    int value;
    int constant;

    Command(vector<int> param1, vector<int> param2, vector<int> param3, vector<string> param4, string param5, int param6, int param7) {
        stagelengths = param1;
        
        bypassindex1 = param2[0];
        bypassindex2 = param2[1];
        readindex = param2[2];
        writeindex = param2[3];

        destinationregister = param3[0];
        sourceregister1 = param3[1];
        sourceregister2 = param3[2];
        constant = param7;

        stagenames = param4;
        opcode = param5;
        value = param6;
    }
};

struct Registerfile{

    int size;
    vector<int> updatetime;
    vector<int> forwarding_updatetime;
    vector<int> values;
    struct UpdateQueue* queue;

    Registerfile(int param1){
        size = param1;
        vector<int> temp1(param1, 0),temp2(param1,0),temp3(param1,0);
        updatetime.resize(size);
        updatetime = temp1;
        forwarding_updatetime.resize(size);
        forwarding_updatetime = temp2;
        values.resize(size);
        values = temp3;
    }

    vector<int> copy_vector(const vector<int>& param1) {
        vector<int> newvector(param1.size());
        for (size_t i = 0; i < param1.size(); i++) {
            newvector[i] = param1[i];
        }
        return newvector;
    }
    
    struct Registerfile* copy_file() {
        struct Registerfile* newregisterfile = new Registerfile(size);
        newregisterfile->size = size;
        newregisterfile->updatetime = copy_vector(updatetime);
        newregisterfile->forwarding_updatetime = copy_vector(forwarding_updatetime);
        newregisterfile->values = copy_vector(values);
        return newregisterfile;
    }

};

struct Runtimedata{

    Command* command;
    int starttime;
    vector<string> stagenames;
    vector<vector<int> > stages;

    Runtimedata(Command* param1, int param2) {
        command = param1;
        starttime = param2;
        stagenames.resize(5);
        vector<string> temp3(5,"");
        stagenames = temp3;
        stages.resize(5);
        vector<vector<int> > temp1(5);
        for (int i = 0; i < 5; i++) {
            vector<int> temp2(2,0);
            temp1[i] = temp2;
        }
        stages = temp1;
    }
    
};

#endif