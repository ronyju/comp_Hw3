//
// Created by Oren on 25/05/2023.
//
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "bp.hpp"
//#include "hw3_output.hpp"
using namespace std;
#ifndef UNTITLED7_TYPES_H
#define UNTITLED7_TYPES_H


/* ------------------------------- Type ------------------------------- */
class Node {
protected:
    int line_number;
    string value;
    string type;
    string label;
    vector<pair<int,BranchLabelIndex>> trueList;
    vector<pair<int,BranchLabelIndex>> falseList;
    vector<pair<int,BranchLabelIndex>> nextList;
    bool hasRet = false;// if this is a function it ends with return. if not need to add ret void.

public:
    // constructor for non-function :
    Node(string type, string value, int line_number) : type(type), value(value), line_number(line_number) {}
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
    vector<pair<int,BranchLabelIndex>> GetNextListToPatch() {return nextList;}
    vector<pair<int,BranchLabelIndex>> GetTrueListToPatch(){return trueList; }
    vector<pair<int,BranchLabelIndex>> GetFalseListToPatch(){return falseList; }
    void SetHasRetToTrue() {hasRet = true;}
    bool GetHasRet() {return hasRet;}
    void SetLabel(string newLabel){label = newLabel;}
    string GetType() { return type; }
    string GetValue() { return value; }
    int GetLineNumber() { return line_number; }
};
#define YYSTYPE Node *


#endif //UNTITLED7_TYPES_H
