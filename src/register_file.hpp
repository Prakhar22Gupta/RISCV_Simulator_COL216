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

    Command(vector<int> param3, vector<int> param4, vector<int> param5, vector<string> param6, string param7, int param8, int param9) {
        stagelengths = param3;
        
        bypassindex1 = param4[0];
        bypassindex2 = param4[1];
        readindex = param4[2];
        writeindex = param4[3];

        destinationregister = param5[0];
        sourceregister1 = param5[1];
        sourceregister2 = param5[2];
        constant = param9;

        stagenames = param6;
        opcode = param7;
        value = param8;
    }
};

struct QueueNode {
    int register_id;
    int value;
    int updatetime;
    QueueNode* next;
    QueueNode* prev;
};

struct UpdateQueue {

    QueueNode* head;
    QueueNode* tail;
    QueueNode* pointer;
    int size;

    UpdateQueue() {
        head = nullptr;
        tail = nullptr;
        pointer = nullptr;
        size = 0;
    }

    void enqueue(int param1,int param2,int param3) {
        QueueNode* newnode = new QueueNode;
        newnode->register_id = param1;
        newnode->value = param2;
        newnode->updatetime = param3;
        newnode->next = nullptr;
        newnode->prev = nullptr;
        if (head == nullptr) {
            head = newnode;
            pointer = newnode;
            tail = newnode;
        }
        else {
            tail->next = newnode;
            newnode->prev = tail;
            tail = newnode;
        }
        size++;
    }

    vector<int> dequeue() {
        if (head == nullptr) {
            return {-1,-1};
        }
        QueueNode* oldhead = head;
        vector<int> data= {oldhead->value,oldhead->register_id};
        head = head->next;
        if(pointer == oldhead){
            pointer = head;
        }
        if (head == nullptr) {
            tail = nullptr;
        }
        delete oldhead;
        size--;
        return data;
    }

    vector<int> peek() {
        if (pointer == nullptr) {
            return {-1,-1,-1};
        }
        vector<int> data = {pointer->value,pointer->register_id,pointer->updatetime};
        return data;
    }

    void move_left() {
        if (pointer == nullptr) {
            return;
        }
        pointer = pointer->prev;
    }

    void move_right() {
        if (pointer == nullptr) {
            return;
        }
        pointer = pointer->next;
    }

    bool is_empty() {
        return head == nullptr;
    }

    struct UpdateQueue* copy_queue() {
        struct UpdateQueue* newqueue = new UpdateQueue();
        QueueNode* temp = head;
        while (temp != nullptr) {
            newqueue->enqueue(temp->register_id, temp->value, temp->updatetime);
            temp = temp->next;
        }
        return newqueue;
    }
};

struct Registerfile{

    int size;
    vector<int> updatetime;
    vector<int> intermediateupdatetime;
    vector<int> values;
    struct UpdateQueue* queue;

    Registerfile(int param1){
        size = param1;
        vector<int> temp1(param1, 0),temp2(param1,0),temp3(param1,0);
        updatetime.resize(size);
        updatetime = temp1;
        intermediateupdatetime.resize(size);
        intermediateupdatetime = temp2;
        values.resize(size);
        values = temp3;
        queue = new UpdateQueue();
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
        newregisterfile->intermediateupdatetime = copy_vector(intermediateupdatetime);
        newregisterfile->values = copy_vector(values);
        newregisterfile->queue = queue->copy_queue();
        return newregisterfile;
    }

    void make_update(int param1,int param2, int param3){
        updatetime[param1] = param3;
        values[param1] = param2;
    }

    vector<int> state_at_time(int param1, vector<int> file, int param2){
        vector<int> newregisterfile = copy_vector(file);
        queue->pointer = queue->head;
        vector<int> temp = queue->peek();
        while(temp[0]!=-1){
            if(temp[2]<=param2){
                queue->move_right();
                temp = queue->peek();
            }
            else if(temp[2]<=param1){
                newregisterfile[temp[1]] = temp[0];
                queue->move_right();
                temp = queue->peek();
            }
            else{
                break;
            }
        }
        return newregisterfile;
    }
};

struct Runtimedata{

    Command* command;
    int starttime;
    vector<string> stagenames;
    vector<vector<int> > stages;

    Runtimedata(Command* param1, int param2, int param3) {
        command = param1;
        starttime = param2;
        stagenames.resize(param3);
        vector<string> temp3(param3,"");
        stagenames = temp3;
        stages.resize(param3);
        vector<vector<int> > temp1(param3);
        for (int i = 0; i < param3; i++) {
            vector<int> temp2(2,0);
            temp1[i] = temp2;
        }
        stages = temp1;
    }
    
};

#endif
