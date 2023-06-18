//
// Created by Oren on 16/06/2023.
//

#ifndef EMITANDSHIT_H
#define EMITANDSHIT_H

#include <iostream>
#include "bp.hpp"
#include "databases.hpp"
#include "types.h"

typedef enum {BOOL_CTYPE, INT_CTYPE, VOID_CTYPE, BYTE_CTYPE, ERROR_CTYPE} FunCTypes;
typedef enum {SLT, SGT, EQ, SLE, SGE, NE, ERR} LlvmCmpOper;

extern ScopeStack scopStack;

class EmitAndShit {

public:
    CodeBuffer* bufferPtr;

    EmitAndShit() {
        CodeBuffer&  buffer = CodeBuffer::instance();
        this->bufferPtr = &buffer;
    }

    string CreateNewLabel(){
       return bufferPtr->genLabel();
    }

    vector<pair<int,BranchLabelIndex>> CreateNewPlaceToPatch() {
        int theHole = bufferPtr->emit("br label @");
        return bufferPtr->makelist(make_pair(theHole,FIRST));
    }

    void SetGlobalAndPrintToBuffer(){
        AddGlobalsToBuffer();
        bufferPtr->printGlobalBuffer();
    }

    void PrintBuffer() {
        bufferPtr->printCodeBuffer();
    }

    void AddGlobalsToBuffer(){
        CodeBuffer &codeBuff = CodeBuffer::instance();
        // constants declaration
        bufferPtr->emitGlobal(R"(@.div_by_zero_err_msg = constant [23 x i8] c"Error division by zero\00")");
        bufferPtr->emitGlobal(R"(@.int_specifier = constant [4 x i8] c"%d\0A\00")");
        bufferPtr->emitGlobal(R"(@.str_specifier = constant [4 x i8] c"%s\0A\00")");
        // function declaration
        bufferPtr->emitGlobal("declare void @exit(i32)");
        bufferPtr->emitGlobal("declare i32 @printf(i8*, ...)");
        //print implementation
        bufferPtr->emitGlobal("define void @print(i8*) {");
        bufferPtr->emitGlobal("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
        bufferPtr->emitGlobal("call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
        bufferPtr->emitGlobal("ret void");
        bufferPtr->emitGlobal("}");
        //printi implementation
        bufferPtr->emitGlobal("define void @printi(i32) {");
        bufferPtr->emitGlobal("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
        bufferPtr->emitGlobal("call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
        bufferPtr->emitGlobal("ret void");
        bufferPtr->emitGlobal("}");
    }

    FunCTypes convertStringToFunCTypes(string type) {
        if (type == "INT") {
            return INT_CTYPE;
        }
        if (type == "BOOL") {
            return BOOL_CTYPE;
        }
        if (type == "BYTE") {
            return BYTE_CTYPE;
        }
        if (type == "VOID") {
            return VOID_CTYPE;
        }
        std::cout << "debug error type doesnt exist -" << type << std::endl; //TODO: remove before submit?
        return ERROR_CTYPE;
    }

    string ConvertTypeToLLVM (string type_str){
        FunCTypes type = convertStringToFunCTypes (type_str);
        switch (type) {
            case INT_CTYPE:     return "i32";
            case BOOL_CTYPE:    return "i1";
            case VOID_CTYPE:    return "void";
            case BYTE_CTYPE:    return "i8";
            default:      {std::cout << "debug error type doesnt exist -" << type << std::endl;  return "ERROR";} //TODO: remove before submit
        }
    }

    void EmitEndOfFunc() {
        bufferPtr->emit("}");
    }

    string ConvertArgsToLLVM (vector<pair<string,string>> types_names_arg_vector) {
        string args = "";
        for (auto pair : types_names_arg_vector) {
            string llvmType = ConvertTypeToLLVM(pair.first);
            int varNum = bufferPtr->GetFreshVar();
            string varName = "%t" + std::to_string(varNum);
            args = args + llvmType +" "+ varName + ", ";
            bool found;
            SymbolTableElement* element = scopStack.SearchInAllScopesByName(pair.second, &found);
            if (!found) { //TODO: remove before submit
                std::cout <<" error in ConvertArgsToLLVM couldn't find element of name "<< pair.second<< endl;
            }
            element->SetReg(varNum);
        }
        if(!types_names_arg_vector.empty()) { args.erase(args.size()-2); }
        return args;
    }

    void FuncDeclareEmitToBuff (string returnType, string funcName, vector<pair<string,string>> types_names_arg_vector){
        string llvmRetType = ConvertTypeToLLVM(returnType);
        string allArgs = ConvertArgsToLLVM(types_names_arg_vector);
        bufferPtr->emit("define " + llvmRetType + " @" + funcName + "(" + allArgs + ") {");
    }

    string GetRegName(int regNum){
        return "%t"+std::to_string(regNum);
    }

    void IdDeclareEmitToBuff (string typeName, string varName) {
        int idRegNum = bufferPtr->GetFreshVar();
        string llvmType = ConvertTypeToLLVM(typeName);
        string defaultType = "0";
        if(typeName == "BOOL")
        {
            int temp_var = bufferPtr->GetFreshVar();
            defaultType = GetRegName(temp_var) + " = zext i1 false to i32";

        }
        bufferPtr->emit(GetRegName(idRegNum) + " = alloca i32");
        bufferPtr->emit("store i32 "+ defaultType +", i32* " + GetRegName(idRegNum));
        // update the id :
        bool found;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(varName, &found);
        if (!found) { //TODO: remove before submit
            std::cout <<" error in IdDeclareEmitToBuff couldn't find element of name "<< varName << endl;
        }
        element->SetReg(idRegNum);
    }

    void IdAssignEmitToBuff (string typeName, string varName,string value,string valueType) {
        std::cout <<"IdAssignEmitToBuff typeName = "<< typeName << " varName = "<< varName<< " value = "<< value<< " valueType = "<<valueType << std::endl; //TODO: remove before submit
        int idRegNum = bufferPtr->GetFreshVar();
        int valueRegNum = bufferPtr->GetFreshVar();
        string newValue = value;
        string llvmType = ConvertTypeToLLVM(typeName);

        bufferPtr->emit(GetRegName(idRegNum) + " = alloca i32");

        if (valueType == "ID") {
//TODO: fill up with load in the middle
        } else {
            if (valueType == "BOOL") {
                int boolVar = bufferPtr->GetFreshVar();
                newValue = GetRegName(boolVar);
                bufferPtr->emit(GetRegName(boolVar) + " = zext i1 " + value +" to i32");
            } else if (valueType == "BYTE") {
                int byteVar = bufferPtr->GetFreshVar();
                newValue = GetRegName(byteVar);
                bufferPtr->emit(GetRegName(byteVar) + " = zext i8 " + value + " to i32");
            }
            bufferPtr->emit("store i32 " + newValue + ", i32* " + GetRegName(idRegNum));
        }
        // update the id :
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(varName, &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in IdAssignEmitToBuff couldn't find element of name " << varName << endl;
        }
        element->SetReg(idRegNum);
    }

    void Patch(const string label,vector<pair<int,BranchLabelIndex>> nextListToPatch ){
        bufferPtr->bpatch(nextListToPatch,label);
    }

    LlvmCmpOper StrToLlvmCmpOper (string str) {
        if (str == "==") { return EQ; }
        if (str == "!=") { return NE; }
        if (str == "<") { return SLT; }
        if (str == ">") { return SGT; }
        if (str == "<=") { return SLE; }
        if (str == ">=") { return SGE; }
        cout << "error unknown oper in strToLlvmCmpOper = " << str <<endl; //TODO: remove before submit
        return ERR;

    }
    string OperToLlvm(string compare)
    {
        LlvmCmpOper oper = StrToLlvmCmpOper(compare);
        switch (oper) {
            case EQ : return "eq";
            case NE : return "ne";
            case SLT : return "slt";
            case SGT : return "sgt";
            case SLE : return "sle";
            case SGE : return "sge";
            default:  cout << "error unknown oper in strToLlvmCmpOper = " << compare <<endl; //TODO: remove before submit
        }
        return "";
    }

    int EmitRelational (string leftValue, string leftType, string rightValue, string rightType, string oper) {
        // TODO: if one of them or both is ID change leftValue and  rightValue to its curRegister
        string resultReg =  GetRegName(bufferPtr->GetFreshVar());
        string llvmOper = OperToLlvm(oper);

        if(rightType == "BYTE") {
            string newRightValue = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(newRightValue + " = zext i8 " + rightValue + " to i32");
            rightValue = newRightValue;
        }
        if(leftType == "BYTE") {
            string newLeftValue = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(newLeftValue + " = zext i8 " + leftValue + " to i32");
            leftValue = newLeftValue;
        }

        bufferPtr->emit(resultReg + " = icmp " + llvmOper + " i32 " + leftValue + " , " + rightValue);

        return BooleanBranchJumpEmit(resultReg);

    }

    int BooleanBranchJumpEmit(string boolReg ) {
        int addressOfHole =  bufferPtr->emit("br i1 " + boolReg + " , label @, label @");
        return addressOfHole;
    }

    void AddRetVoidIfNeeded (bool funcHasRet) {
        if (!funcHasRet) {
            bufferPtr->emit ("ret void");
        }
    }

};


#endif //EMITANDSHIT_H
