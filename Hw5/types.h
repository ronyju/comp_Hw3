//
// Created by Oren on 25/05/2023.
//
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "bp.hpp"
//#include "hw3_output.hpp"
#include "Register.h"
using namespace std;
#ifndef UNTITLED7_TYPES_H
#define UNTITLED7_TYPES_H
/* ------------------------------- for brake and continue ------------------------------- */

class loopNextListAndLable {
    protected:
    string lable; // the lable of the while bool check for contiue
    vector<pair<int,BranchLabelIndex>> nextList; //for break
    
    public:
    loopNextListAndLable (){};
    loopNextListAndLable(string lable) : lable(lable) {}
    loopNextListAndLable (vector<pair<int,BranchLabelIndex>> &nextList) : nextList(nextList) {};
    loopNextListAndLable (string lable, vector<pair<int,BranchLabelIndex>> &nextList) : lable (lable), nextList(nextList){}

    void SetNextList(vector<pair<int,BranchLabelIndex>> &newList){
        nextList = newList;
    }
    void SetLable (string newLable) { lable = newLable; } 
    void MergeToNextList(const vector<pair<int,BranchLabelIndex>> &newListToMerge) {
        vector<pair<int,BranchLabelIndex>> newList(nextList.begin(),nextList.end());
        newList.insert(newList.end(),newListToMerge.begin(),newListToMerge.end());
        nextList =  newList;
    }
    string GetLable() { return lable; }
    vector<pair<int,BranchLabelIndex>> GetNextList() { return nextList; }
};


/* ------------------------------- Type ------------------------------- */
class Node {
protected:
    int line_number;
    string value;
    string type;
    string label;
    Register reg;
    vector<pair<int,BranchLabelIndex>> trueList;
    vector<pair<int,BranchLabelIndex>> falseList;
    vector<pair<int,BranchLabelIndex>> nextList;
    bool hasRet = false;// if this is a function it ends with return. if not need to add ret void.

public:
    // constructor for non-function :
    Node(string type, string value, int line_number,string reg_value= "REG_UNDEF") : type(type), value(value), line_number(line_number),reg(reg_value) {}
    virtual ~Node() {};
    void MergeToNextList(const vector<pair<int,BranchLabelIndex>> &newListToMerge){
        vector<pair<int,BranchLabelIndex>> newList(nextList.begin(),nextList.end());
        newList.insert(newList.end(),newListToMerge.begin(),newListToMerge.end());
        nextList =  newList;
    }
    void MergeToTrueList(const vector<pair<int,BranchLabelIndex>> &newListToMerge){
        vector<pair<int,BranchLabelIndex>> newList(trueList.begin(),trueList.end());
        newList.insert(newList.end(),newListToMerge.begin(),newListToMerge.end());
        trueList =  newList;
    }
    void MergeToFalseList(const vector<pair<int,BranchLabelIndex>> &newListToMerge){
        vector<pair<int,BranchLabelIndex>> newList(falseList.begin(),falseList.end());
        newList.insert(newList.end(),newListToMerge.begin(),newListToMerge.end());
        falseList =  newList;
    }
    string GetLabel(){return label;}
    void SetNextList(vector<pair<int,BranchLabelIndex>> newNextList) {nextList = newNextList;}
    void SetTrueList(vector<pair<int,BranchLabelIndex>> newTrueList) {trueList = newTrueList;}
    void SetFalseList(vector<pair<int,BranchLabelIndex>> newFalseList) {falseList = newFalseList;}
    vector<pair<int,BranchLabelIndex>> GetNextListToPatch() {return nextList;}
    vector<pair<int,BranchLabelIndex>> GetTrueListToPatch(){return trueList; }
    vector<pair<int,BranchLabelIndex>> GetFalseListToPatch(){return falseList; }
    void SetHasRetToTrue() {hasRet = true;}
    bool GetHasRet() {return hasRet;}
    void SetLabel(string newLabel){label = newLabel;}
    string GetType() { return type; }
    string GetValue() { return value; }
    void SetType(string newType) { type = newType; }
    int GetLineNumber() { return line_number; }
    string GetRegName() { return reg.GetRegister(); }
    Register GetReg() { return reg; }
    void SetReg(int reg_num){reg.SetRegister(reg_num);}
    void SetReg(string reg_value){reg.SetRegister(reg_value);}
    void SetIsString(bool is) { reg.SetIsString(is); }
    void SetStringLength (int length) { reg.SetStringLength(length); }
    bool IsString () { return reg.IsString(); }
    int GetStringLength() { return reg.GetStringLength(); }

};
#define YYSTYPE Node *


#endif //UNTITLED7_TYPES_H
