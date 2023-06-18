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

    string GetRegFromId (string idName) {
        // this function is looking for an ID in the scope, and return its curent register.
        bool found;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(idName, &found);
        if (!found) {
            std::cout << "elemnt with id name = " << idName <<" not found! " << std::endl; //TODO: remove before submit
            return "";
        }
        if (element->GetReg() == REG_UNDEF) {
            std::cout << "elemnt with id name = " << idName <<" register is undef " << std::endl; //TODO: remove before submit
            return "";
        }
        return GetRegName(element->GetReg());
    }


    string GetTypeFromId (string idName) {
        // this function is looking for an ID in the scope, and return its curent register.
        bool found;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(idName, &found);
        if (!found) {
            std::cout << "elemnt with id name = " << idName <<" not found! " << std::endl; //TODO: remove before submit
            return "";
        }
        return element->GetType().GetTypeName();
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

    string IdDeclareEmitToBuff (string typeName, string varName) {
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
        return GetRegName(idRegNum);
    }

    string IdAssignEmitToBuff (string typeName, string varName,string value,string valueType, string valueReg) {
        std::cout << "rony. valueReg = "<< valueReg <<" and valueType = "<< valueType << " and value = " << value<<endl; //TODO: remove befor submit 
        int idRegNum = bufferPtr->GetFreshVar();
        int valueRegNum = bufferPtr->GetFreshVar();
        string newValue = value;
        bool isID;
        if (value == "NONE") { newValue = valueReg; value = valueReg; isID = true;} // for example int x = 3 + 5 , EXP = 3+5. EXP->GetValue() = "NONE" and its reg was updated.
        string llvmType = ConvertTypeToLLVM(typeName);

        bufferPtr->emit(GetRegName(idRegNum) + " = alloca i32");

        if (valueType == "ID") {
//TODO: fill up with load in the middle
        } else { // if isID it was already converted to i32!
            if (valueType == "BOOL" && !isID) {
                int boolVar = bufferPtr->GetFreshVar();
                newValue = GetRegName(boolVar);
                bufferPtr->emit(GetRegName(boolVar) + " = zext i1 " + value +" to i32");
            } else if (valueType == "BYTE" && !isID) {
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
        return GetRegName(idRegNum);
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

    int EmitRelational (string leftValue, string leftType, string rightValue, string rightType, string oper, string& resultRegName) {
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
        resultRegName = resultReg;
        return BooleanBranchJumpEmit(resultReg);
    }

    int BooleanBranchJumpEmit(string boolReg) {
        int addressOfHole =  bufferPtr->emit("br i1 " + boolReg + " , label @, label @");
        return addressOfHole;
    }

    void AddRetVoidIfNeeded (bool funcHasRet) {
        if (!funcHasRet) {
            bufferPtr->emit ("ret void");
        }
    }

    string GetOpFromSign (string sign) {
        if (sign == "+") {return "add";}
        if (sign == "-") {return "sub";}
        if (sign == "*") {return "mul";}
        if (sign == "/") {return "sub";}
        std::cout <<"Error in GetOpFromSign op - " << sign <<" is unknown";
        return "";
    }

    string AddetiveEmit (string sign , string leftValue, string leftType, string rightValue , string rightType) {
        // TODO: if they are ID and not TYPE - must convert.
        bool leftIsID = (leftType == "ID");  
        bool rightIsID = (rightType == "ID");  
        if (leftIsID) {
            string leftIdName = leftValue;
            leftType = GetTypeFromId(leftIdName);
            leftValue = GetRegFromId(leftIdName);
        }

        if (rightIsID) {
            string rightIdName = rightValue;
            rightType = GetTypeFromId(rightIdName);
            rightValue = GetRegFromId(rightIdName);
        }

        string newVar = GetRegName(bufferPtr->GetFreshVar());
        string operation = GetOpFromSign(sign);
        bool bothSidesAreByte = (leftType == "BYTE") && (rightType == "BYTE");
        bool onlyRightIsByte = (leftType != "BYTE") && (rightType == "BYTE");
        bool onlyLeftIsByte = (leftType == "BYTE") && (rightType != "BYTE");
        bool bothSidesAreINT = (leftType != "BYTE") && (rightType != "BYTE");

        if (bothSidesAreByte && !leftIsID && !rightIsID) { // result is BYTE:
            // this is added because maybe the value is stored in a reg that was alloca, and then its i32* and not i32.
            string tempRightReg = GetRegName(bufferPtr->GetFreshVar());
            string templeftReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(tempRightReg + " = load i8, i8* " + rightValue ); 
            bufferPtr->emit(templeftReg + " = load i8, i8* " + leftValue );
            
            bufferPtr->emit(newVar + " = " + operation + " i8 " + templeftReg + " , " + tempRightReg );
        }
        if (onlyRightIsByte && !rightIsID) { // convert the right side to i32
            string convertedRightReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(convertedRightReg + " = zext i8 " + rightValue + " to i32");

             // this is added because maybe the value is stored in a reg that was alloca, and then its i32* and not i32.
            string templeftReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(templeftReg + " = load i32, i32* " + leftValue );
            
            bufferPtr->emit(newVar + " = " + operation + " i32 " + templeftReg + " , " + convertedRightReg);
        }
        if (onlyLeftIsByte && !leftIsID) { // convert the left side to i32
            string convertedLeftReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(convertedLeftReg + " = zext i8 " + leftValue + " to i32");

            // this is added because maybe the value is stored in a reg that was alloca, and then its i32* and not i32.
            string tempRightReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(tempRightReg + " = load i32, i32* " + rightValue ); 
            
            bufferPtr->emit(newVar + " = " + operation + " i32 " + convertedLeftReg + " , " + tempRightReg);
        }
        if (bothSidesAreINT || (onlyLeftIsByte && leftIsID) || (onlyRightIsByte && rightIsID ) || (bothSidesAreByte && leftIsID && rightIsID)) {
             // this is added because maybe the value is stored in a reg that was alloca, and then its i32* and not i32.
            string tempRightReg = GetRegName(bufferPtr->GetFreshVar());
            string templeftReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(tempRightReg + " = load i32, i32* " + rightValue ); 
            bufferPtr->emit(templeftReg + " = load i32, i32* " + leftValue );
            
            bufferPtr->emit(newVar + " = " + operation + " i32 " + templeftReg + " , " + tempRightReg );
        }
        return newVar; // in the parser - will set the mother statment to this register. 
    }
};


#endif //EMITANDSHIT_H
