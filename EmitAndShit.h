//
// Created by Oren on 16/06/2023.
//

#ifndef EMITANDSHIT_H
#define EMITANDSHIT_H

#include <iostream>
#include "bp.hpp"
#include "databases.hpp"
#include "types.h"
#include "Register.h"
#define MAX_BYTE 255

typedef enum {BOOL_CTYPE, INT_CTYPE, VOID_CTYPE, BYTE_CTYPE, ERROR_CTYPE} FunCTypes;
typedef enum {SLT, SGT, EQ, SLE, SGE, NE, ERR} LlvmCmpOper;

extern ScopeStack scopStack;

class EmitAndShit {

public:
    vector<int> alloca_numbers;
    CodeBuffer *bufferPtr;

    bool IsRegAllocaInt(int reg){
        if (reg == -1) {return false;}
        for (auto num : alloca_numbers){
            if (num == reg) {return true;}
        }
        return false;
    }

    int GetNumRegFromRegName(string regName){
        if (regName.length() < 3) {return -1;}
        if (regName.substr(0,2) != "%t") {return -1;}
        string numStr = regName.substr(2);
        int regNum = std::atoi(numStr.c_str());
        return regNum;
    }

    bool IsRegAlloca(string regName){
        return IsRegAllocaInt(GetNumRegFromRegName(regName));
    }

    EmitAndShit() {
        CodeBuffer &buffer = CodeBuffer::instance();
        this->bufferPtr = &buffer;
    }
    
    string LoadIdPointerRegToTemp(string idPointerReg){
        // gets an idPinter reg, and load to a temp reg. return the temp reg name
        string tempReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(tempReg + " = load i32, i32* " + idPointerReg);
        return tempReg;
    }

    void StoreFromTempToIdPointerReg(string tempReg, string idPointerReg) {
        bufferPtr->emit("store i32 " + tempReg + ", i32* " + idPointerReg);
    }

    string CreateNewLabel() {
        return bufferPtr->genLabel();
    }

    vector<pair<int, BranchLabelIndex>> CreateNewPlaceToPatch() {
        int theHole = bufferPtr->emit("br label @");
        return bufferPtr->makelist(make_pair(theHole, FIRST));
    }

    void SetGlobalAndPrintToBuffer() {
        AddGlobalsToBuffer();
        bufferPtr->printGlobalBuffer();
    }

    void PrintBuffer() {
        //std::cout << "-----------Print Buffer:-----------"<<std::endl; //TODO: remove before submit
        bufferPtr->printCodeBuffer();
    }

    void AddGlobalsToBuffer() {
        CodeBuffer &codeBuff = CodeBuffer::instance();
        // constants declaration
        bufferPtr->emitGlobal(R"(@.div_by_zero_error_message = constant [23 x i8] c"Error division by zero\00")");
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

    string GetTypeFromIdName(string idName) {
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(idName, &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in GetTypeFromIdName couldn't find element of name " << idName << endl;
        }
        return element->GetType().GetTypeName();
    }

    string ConvertTypeToLLVM(string type_str, string name = " ") {
        if (type_str == "ID") { type_str = GetTypeFromIdName(name); }
        FunCTypes type = convertStringToFunCTypes(type_str);
        switch (type) {
            case INT_CTYPE:
                return "i32";
            case BOOL_CTYPE:
                return "i1";
            case VOID_CTYPE:
                return "void";
            case BYTE_CTYPE:
                return "i8";
            default: {
                std::cout << "debug error type doesnt exist -" << type << std::endl;
                return "ERROR";
            } //TODO: remove before submit
        }
    }

    void EmitEndOfFunc() {
        bufferPtr->emit("}");
    }

    string GetRegFromIdName(string idName) {
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(idName, &found);
        if (!found) return "REG_UNDEF"; // the id is not yet in the scope
        return element->GetReg();
    }

    string ConvertArgsToLLVM(vector<string> argsRegs) {
        string args = "";
        for (auto argReg: argsRegs) {
            string llvmType = "i32";
            string varName = argReg;
            args = args + llvmType + " " + varName + ", ";
        }
        if (!argsRegs.empty()) { args.erase(args.size() - 2); }
        return args;
    }

    void  FuncDeclareEmitToBuff(string returnType, string funcName, vector<pair<string, string>> types_names_arg_vector) {
        vector<string> arguments_names;
        for (auto pair: types_names_arg_vector) {
            //std::cout << " pair.first = "<<  pair.first << "  pair.second " << pair.second << ", " ;
            arguments_names.push_back(pair.second);
        }
        std::cout << endl;
        vector<string> arguments_regs = CreateArgsRegs(arguments_names);
        string llvmRetType = ConvertTypeToLLVM(returnType);
        string allArgs = ConvertArgsToLLVM(arguments_regs);
        bufferPtr->emit("define " + llvmRetType + " @" + funcName + "(" + allArgs + ") {");
    }

    string GetRegName(int regNum) {
        return "%t" + std::to_string(regNum);
    }

    string IdDeclareEmitToBuff(string typeName, string varName) { // Type ID SC
        int idRegNumAlloca = bufferPtr->GetFreshVar();
        string llvmType = ConvertTypeToLLVM(typeName);
        string theDefaultValue = "0"; // default val for int or byte

        bufferPtr->emit(GetRegName(idRegNumAlloca) + " = alloca i32");
        alloca_numbers.push_back(idRegNumAlloca);

        if (typeName == "BOOL") {
            int temp_var = bufferPtr->GetFreshVar();
            bufferPtr->emit( GetRegName(temp_var) + " = zext i1 false to i32");
            bufferPtr->emit("store i32 " + GetRegName(temp_var) + ", i32* " + GetRegName(idRegNumAlloca));

        }else {        
            bufferPtr->emit("store i32 " + theDefaultValue + ", i32* " + GetRegName(idRegNumAlloca));
        }

        int idRegNum = bufferPtr->GetFreshVar();
        bufferPtr->emit(GetRegName(idRegNum) + " = load i32, i32* " + GetRegName(idRegNumAlloca));
        // update the id :
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(varName, &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in IdDeclareEmitToBuff couldn't find element of name " << varName << endl;
        }
        //Rony - changed- returning the alloca value.
        element->SetReg(idRegNumAlloca, true);//element->SetReg(idRegNum);
        return GetRegName(idRegNumAlloca);//return GetRegName(idRegNum);
    }

    string GetTypeFromId(string idName) {
        // this function is looking for an ID in the scope, and return its type name for example "INT" "BOOL"
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(idName, &found);
        if (!found) {
            std::cout << "elemnt with id name = " << idName << " not found! " << std::endl; //TODO: remove before submit
            return "";
        }
        return element->GetType().GetTypeName();
    }

    string IdAssignEmitToBuff(string typeName, string varName, string value, string valueType) { //Type ID ASSIGN Exp SC  - the first assighment
        if (typeName == "ID") { typeName = GetTypeFromIdName(varName); }
        int idRegNumAlloca = bufferPtr->GetFreshVar();
        int idRegNum = bufferPtr->GetFreshVar();
        
        string llvmType = ConvertTypeToLLVM(typeName);

        //Rony added - if value is an ID it's register is the alloca register - so we need to store it :
        if (valueType == "ID" && IsRegAlloca(value)){
            value = LoadIdPointerRegToTemp(value);
        }

        string newValue = value;
        bufferPtr->emit(GetRegName(idRegNumAlloca) + " = alloca i32");
        alloca_numbers.push_back(idRegNumAlloca);

        // int x = y or int x = 5; or int x = 5+4  (than value will be %ti)
        //TODO: can we never zext?
        if (valueType == "BOOL") {
            int boolVar = bufferPtr->GetFreshVar();
            newValue = GetRegName(boolVar);
            bufferPtr->emit(newValue + " = zext i1 " + value + " to i32");

        }/*} else if (valueType == "BYTE") {
            int byteVar = bufferPtr->GetFreshVar();
            newValue = GetRegName(byteVar);
            bufferPtr->emit(GetRegName(byteVar) + " = zext i8 " + value + " to i32");
        }*/
        bufferPtr->emit("store i32 " + newValue + ", i32* " + GetRegName(idRegNumAlloca));


        bufferPtr->emit(GetRegName(idRegNum) + " = load i32, i32* " + GetRegName(idRegNumAlloca));
        // update the id :
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(varName, &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in IdAssignEmitToBuff couldn't find element of name " << varName << endl;
        }

        //Rony - changed- returning the alloca value.
        element->SetReg(idRegNumAlloca, true); //element->SetReg(idRegNum);
        return GetRegName(idRegNumAlloca);// return GetRegName(idRegNum);
    }

    void Patch(const string label, vector<pair<int, BranchLabelIndex>> nextListToPatch) {
        bufferPtr->bpatch(nextListToPatch, label);
    }

    LlvmCmpOper StrToLlvmCmpOper(string str) {
        if (str == "==") { return EQ; }
        if (str == "!=") { return NE; }
        if (str == "<") { return SLT; }
        if (str == ">") { return SGT; }
        if (str == "<=") { return SLE; }
        if (str == ">=") { return SGE; }
        cout << "error unknown oper in strToLlvmCmpOper = " << str << endl; //TODO: remove before submit
        return ERR;

    }

    string OperToLlvm(string compare) {
        LlvmCmpOper oper = StrToLlvmCmpOper(compare);
        switch (oper) {
            case EQ :
                return "eq";
            case NE :
                return "ne";
            case SLT :
                return "slt";
            case SGT :
                return "sgt";
            case SLE :
                return "sle";
            case SGE :
                return "sge";
            default:
                cout << "error unknown oper in strToLlvmCmpOper = " << compare << endl; //TODO: remove before submit
        }
        return "";
    }

    void AddRetVoidIfNeeded(bool funcHasRet) {
        if (!funcHasRet) {
            bufferPtr->emit("ret void");
        }
    }

    void AddRetIfNeeded(bool funcHasRet, string retType) {
        if (!funcHasRet) {
            DefultReturnEmit(retType);
        }
    }

    int EmitRelational(Node* Mother, string leftValue, string leftType, string rightValue, string rightType, string oper) {
        string resultReg = GetRegName(bufferPtr->GetFreshVar());
        string llvmOper = OperToLlvm(oper);
//TODO: can we never zext?
/*
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
        */

       //Rony added if left or right are ID we need to load them :
        if (rightType == "ID" && IsRegAlloca(rightValue)){
            rightValue = LoadIdPointerRegToTemp(rightValue);
        }
        if (leftType == "ID"  && IsRegAlloca(leftValue)){
            leftValue = LoadIdPointerRegToTemp(leftValue);
        }


        bufferPtr->emit(resultReg + " = icmp " + llvmOper + " i32 " + leftValue + " , " + rightValue);

        return BooleanBranchJumpEmit(resultReg);

    }

    int BooleanBranchJumpEmit(string boolReg) {
        //std::cout << "RONY the holes came from BooleanBranchJumpEmit" <<std::endl;
        int addressOfHole = bufferPtr->emit("br i1 " + boolReg + " , label @, label @");
        return addressOfHole;
    }

    vector<string> CreateArgsRegs(vector<string> arguments_names) {
        vector<string> argumentsRegs;
        for (auto name: arguments_names) {
            int reg = bufferPtr->GetFreshVar();
            bool found;
            SymbolTableElement *element = scopStack.SearchInAllScopesByName(name, &found);
            if (!found || element == NULL) { //TODO: remove before submit
                std::cout << " error in CreateArgsRegs couldn't find element of name " << name << endl;
                return argumentsRegs;
            }
            element->SetReg(reg);
            argumentsRegs.push_back(GetRegName(reg));
        }
        return argumentsRegs;
    }

    void DefultReturnEmit(string FuncRetType) {
        if (FuncRetType == "VOID") { bufferPtr->emit("ret void"); }
        if (FuncRetType == "INT") { bufferPtr->emit("ret i32 0"); }
        if (FuncRetType == "BYTE") { bufferPtr->emit("ret i8 0"); }
        if (FuncRetType == "BOOL") { bufferPtr->emit("ret i1 false"); }
    }

    void EmitDivByZeroCheck(string type, string value, string FuncRetType, string name) {
        // print a code that checks if the value is 0 and if so print error and exit.
        string checkRegister = GetRegName(bufferPtr->GetFreshVar());
        
        //Rony added - if type is ID call load first:
        if (type == "ID" && IsRegAlloca(value)) {
            value = LoadIdPointerRegToTemp(value);
        }
        
        type = ConvertTypeToLLVM(type, name);
        bufferPtr->emit(checkRegister + " = icmp eq i32 " + value +", 0");//value + ", 0"); 
        int lineToPatch = bufferPtr->emit("br i1 " + checkRegister + ", label @, label @");
        vector<pair<int, BranchLabelIndex>> trueListOfAddressToPatch = bufferPtr->makelist({lineToPatch, FIRST});
        vector<pair<int, BranchLabelIndex>> falseListOfAddressToPatch = bufferPtr->makelist({lineToPatch, SECOND});

        string trueLable = CreateNewLabel();
        bufferPtr->emit("call void (i8*) @print(i8* getelementptr ([23 x i8], [23 x i8]* @.div_by_zero_error_message, i32 0, i32 0))");
        bufferPtr->emit("call void (i32) @exit(i32 0)");
        DefultReturnEmit(FuncRetType);

        string falseLabel = CreateNewLabel();
        bufferPtr->bpatch(trueListOfAddressToPatch, trueLable);
        bufferPtr->bpatch(falseListOfAddressToPatch, falseLabel);
    }

    void UpdateListsAndPAtchAnd(Node *leftBool, Node *rightBool, string MLable, Node *motherNode) {
        /*
            Mother ------------------------------
            |       **leftBool**                |
            |   true list-> rightBool             |
            |   false list -> mother false list |
            |                                   |
            |       **rightBool**               |
            |   true list -> Mother true list   |
            |   false list -> mother false list |
            |------------------------------------
        */

        // add the true list of the right bool to the mother true list:
        motherNode->MergeToTrueList(rightBool->GetTrueListToPatch());

        // combine both false list of the bool to the mother false list
        motherNode->MergeToFalseList(leftBool->GetFalseListToPatch());
        motherNode->MergeToFalseList(rightBool->GetFalseListToPatch());

        // if leftBool is true continue to check the right bool:
        bufferPtr->bpatch(leftBool->GetTrueListToPatch(), MLable);
    }

    void UpdateListsAndPAtchOr(Node *leftBool, Node *rightBool, string MLable, Node *motherNode) {
        /*
            Mother ------------------------------
            |       **leftBool**                |
            |   true list-> mother true list    |
            |   false list -> rightBool         |
            |                                   |
            |       **rightBool**               |
            |   true list -> Mother true list   |
            |   false list -> mother false list |
            |------------------------------------
        */

        // add the false list of the right bool to the mother false list:
        motherNode->MergeToFalseList(rightBool->GetFalseListToPatch());

        // combine both true list of the bool to the mother true list
        motherNode->MergeToTrueList(rightBool->GetTrueListToPatch());
        motherNode->MergeToTrueList(leftBool->GetTrueListToPatch());

        // if leftBool is false continue to check the right bool:
        bufferPtr->bpatch(leftBool->GetFalseListToPatch(), MLable);


    }

    string EmitCheckForOverFlowByte(string resultReg) {
        // will trunc than zext and return the new reg
        string truncReg = GetRegName(bufferPtr->GetFreshVar());
        string zextReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(truncReg + " = trunc i32 " + resultReg + " to i8");

        bufferPtr->emit(zextReg + " = zext i8 " + truncReg + " to i32");
        return zextReg;
    }

    string GetOpFromSign(string sign) {
        if (sign == "+") { return "add"; }
        if (sign == "-") { return "sub"; }
        if (sign == "*") { return "mul"; }
        if (sign == "/") { return "sdiv"; }
        std::cout << "Error in GetOpFromSign op - " << sign << " is unknown";
        return "";
    }

    string AddetiveAndMultiplicativeEmit(string sign, Node *leftNode, Node *rightNode, string funcRetType) {
        
        // get reg will put in value ether a register if it has (%t4)or a number (5)
        string rightValue = rightNode->GetRegName();
        string leftValue = leftNode->GetRegName();
        
        string rightType = rightNode->GetType();
        string originalRightType = rightType;
        if (rightType == "ID") {
            rightType = GetTypeFromId(rightNode->GetValue());
            if (IsRegAlloca(rightValue)) {rightValue = LoadIdPointerRegToTemp(rightValue);} // Rony added 
        }
        string leftType = leftNode->GetType();
        if (leftType == "ID") {
            leftType = GetTypeFromId(leftNode->GetValue());
            if (IsRegAlloca(leftValue)) {leftValue = LoadIdPointerRegToTemp(leftValue); }// Rony added 
        }
        
        string tempReg = GetRegName(bufferPtr->GetFreshVar());
        string operation = GetOpFromSign(sign);
        if (operation == "sdiv") { EmitDivByZeroCheck(originalRightType, rightValue, funcRetType, rightNode->GetValue()); }


        // as if it is INT:
        bool bothSidesAreByte = (leftType == "BYTE") && (rightType == "BYTE");

        bufferPtr->emit(tempReg + " = " + operation + " i32 " + leftValue + " , " + rightValue);
        if (leftType == "BYTE" && rightType == "BYTE") { tempReg = EmitCheckForOverFlowByte(tempReg); }
        return tempReg; // in the parser - will set the mother statement to this register.
    }

    string EmitCallFuncNoArgs(Node *funcId) {
        bool found;
        SymbolTableElement *funElement = scopStack.SearchInAllScopesByName(funcId->GetValue(), &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in EmitCallFuncNoArgs couldn't find element of name " << funcId << endl;
        }
        string returnType = funElement->GetType().GetReturnType();
        string funcName = funElement->GetName();
        string resultReg = "REG_UNDEF";
        string call;
        if (returnType != "VOID") {
            resultReg = GetRegName(bufferPtr->GetFreshVar());
            call = resultReg + " = ";
        }
        call += "call " + ConvertTypeToLLVM(returnType);

        bufferPtr->emit(call + " @" + funcName + "()");
        return resultReg;
    }

    void EmitStringToGlobalBuffer(Node *strNode, Node *MotherNode) {
        int newLength = strNode->GetValue().length() - 2;
        string str = strNode->GetValue().substr(1, newLength);
        string globalReg = "@.t" + to_string(bufferPtr->GetFreshVar());

        bufferPtr->emitGlobal(
                globalReg + " = private unnamed_addr constant [" + to_string(newLength + 1) + " x i8] c\"" + str +
                "\\00\"");
        MotherNode->SetReg(globalReg);
        MotherNode->SetIsString(true);
        MotherNode->SetStringLength(newLength + 1);
    }

    string EmitCallFuncWithArgs(Node *funcId, vector<Register> reg_arg_vector) {
        bool found;
        SymbolTableElement *funElement = scopStack.SearchInAllScopesByName(funcId->GetValue(), &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in EmitCallFuncNoArgs couldn't find element of name " << funcId << endl;
        }
        string returnType = funElement->GetType().GetReturnType();
        string funcName = funElement->GetName();
        if (returnType == "BOOL") {
            //TODO:
        }
        string args = "";
        for (auto argReg: reg_arg_vector) {
            if (argReg.IsString()) { //"STRING" call void @print(i8* getelementptr ([5 x i8], [5 x i8]* @.t0, i32 0, i32 0))
                string length = to_string(argReg.GetStringLength());
                args += "i8* getelementptr ([" + length + " x i8], [" + length + " x i8]* " + argReg.GetRegister() +
                        ", i32 0, i32 0) ,";
            } else {
                string cur_reg = argReg.GetRegister();
                if (argReg.IsAlloca() || IsRegAlloca(cur_reg)) { cur_reg = LoadIdPointerRegToTemp(cur_reg); } //Rony added for registers that are alloca 
                args += "i32 " + cur_reg + ", ";
            }
        }
        // remove last ,:
        args.erase(args.size() - 2);

        string resultReg = "REG_UNDEF";
        string call;
        if (returnType != "VOID") {
            resultReg = GetRegName(bufferPtr->GetFreshVar());
            call = resultReg + " = ";
        }
        call += "call " + ConvertTypeToLLVM(returnType);

        bufferPtr->emit(call + " @" + funcName + "( " + args + " )");
        return resultReg;
    }

    int EmitHardCodedBool() {
        return bufferPtr->emit("br label @");
    }

    int EmitBreak() {
        return bufferPtr->emit("br label @");
    }

    void EmitConinue(string loopLable) {
        bufferPtr->emit("br label %" + loopLable);
    }

    string EmitCastIntToByte(string newType, Node* exp) {
        string expType = exp->GetType();
        if (exp->GetType() == "ID" && IsRegAlloca(exp->GetRegName())) { expType = GetTypeFromIdName(exp->GetValue()); }
        if (newType!= "BYTE" || expType!="INT") {
            return "NOP";
        }
        // this is like :
        // int a = 300;
        // byte b = (byte) a ;
        // we want no b to be 255.
        string expReg = exp->GetRegName();

        //rony added if the type is ID load it 
        if (exp->GetType() == "ID" && IsRegAlloca(expReg)) {expReg = LoadIdPointerRegToTemp(expReg);} 

        // will trunc than zext and return the new reg
        string truncReg = GetRegName(bufferPtr->GetFreshVar());
        string zextReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(truncReg + " = trunc i32 " + expReg + " to i8");

        bufferPtr->emit(zextReg + " = zext i8 " + truncReg + " to i32");
        return zextReg;
    }

    int EmitLable(string res, string& regName, string& labelName, bool givenLable = false) {
        // emiting the lable if res =true its the true lable if res = false its the false lable.
        regName = GetRegName(bufferPtr->GetFreshVar());
        if (!givenLable) {labelName = CreateNewLabel();}
        bufferPtr->emit(regName + " = add i1 0, " + res);
        int HoleLine = bufferPtr->emit("br label @");
        return HoleLine;
    }

    string EmitPhi(string trueHolderReg, string true_label, string falseHolderReg, string false_label) {
        string phi = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(phi + " = phi i1 [" + trueHolderReg + ", %" + true_label + "], [" + falseHolderReg + ", %" + false_label + "]");
        return phi;
    }

    string EmitBoolAssign(string varName, Node* value) // bool x = true;   - or -   x = true;
    {
        int headLable = bufferPtr->emit("br label @");

        string trueLabel = CreateNewLabel();
        bufferPtr->bpatch(bufferPtr->makelist(make_pair(headLable, FIRST)), trueLabel);

        string trueHolderReg;
        int trueHoleLine = EmitLable ("true", trueHolderReg, trueLabel, true);

        string falseLabel;
        string falseHolderReg;
        int falseHoleLine = EmitLable ("false", falseHolderReg, falseLabel);

        string phiLable = CreateNewLabel();
        string phi = EmitPhi(trueHolderReg, trueLabel, falseHolderReg, falseLabel);

        string tempReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(tempReg + " = zext i1 " + phi + " to i32");
        string allocaReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(allocaReg + " = alloca i32");
        alloca_numbers.push_back(GetNumRegFromRegName(allocaReg));

        bufferPtr->emit("store i32 " + tempReg + ", i32* " + allocaReg);

        int idRegNum = bufferPtr->GetFreshVar();
        bufferPtr->emit(GetRegName(idRegNum) + " = load i32, i32* " + allocaReg);

        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(varName, &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in EmitBoolAssign couldn't find element of name " << varName << endl;
        }
        element->SetReg(allocaReg, true);

        bufferPtr->bpatch(bufferPtr->makelist(make_pair(falseHoleLine, FIRST)), phiLable);
        bufferPtr->bpatch(bufferPtr->makelist(make_pair(trueHoleLine, FIRST)), phiLable);
        bufferPtr->bpatch(value->GetFalseListToPatch(), falseLabel);
        bufferPtr->bpatch(value->GetTrueListToPatch(), trueLabel);

        return allocaReg;
    }

    int loadBoolID(string idRegName)
    {
        if (IsRegAlloca(idRegName)) {idRegName = LoadIdPointerRegToTemp(idRegName);}
        string truncReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(truncReg + " = trunc i32 " + idRegName + " to i1");
        string boolReg = GetRegName(bufferPtr->GetFreshVar());
        bufferPtr->emit(boolReg + " = icmp eq i1 " + truncReg + " , true");
        int holeLine = bufferPtr->emit("br i1 " + boolReg + " , label @ , label @");
        return holeLine;
    }

    int EmitBoolFuncCall(string funcName,Register funcRegister) {
        string funcReg = funcRegister.GetRegister();
        if (funcRegister.IsAlloca() || IsRegAlloca(funcRegister.GetRegister())) { funcReg = LoadIdPointerRegToTemp(funcReg); }
        string retType;
        bool found;
        SymbolTableElement *element = scopStack.SearchInAllScopesByName(funcName, &found);
        if (!found) { //TODO: remove before submit
            std::cout << " error in EmitBoolFuncCall couldn't find element of name " << funcName << endl;
        }
        retType = element->GetType().GetReturnType();
        if(retType == "BOOL")
        {
            string comReg = GetRegName(bufferPtr->GetFreshVar());
            bufferPtr->emit(comReg + " = icmp eq i1 " + funcReg + ", true");
            int holeLine = bufferPtr->emit("br i1 " + comReg + ", label @, label @");
            return holeLine;
        }
        return -1;
    
    }

    void EmitReturn(string regName, string regType, Node* exp) {
        if (regType == "ID") {
            string actual_type = GetTypeFromId(exp->GetValue());
            if (IsRegAlloca(regName)) {regName = LoadIdPointerRegToTemp(regName);} //Rony added 
            if (actual_type != "BOOL") {bufferPtr->emit("ret i32 " + regName);}
            else                       {returnBoolean(regName, exp);}
        }
        if (regType == "INT") { bufferPtr->emit("ret i32 " + regName); }
        if (regType == "BYTE") { bufferPtr->emit("ret i8 " + regName); }
        if (regType == "BOOL") { if (exp->IsTrueOrFalse()) {bufferPtr->emit("ret i1 " + regName);} else {returnBoolean(regName, exp);} }
    }

    void returnBoolean (string regName, Node* exp) {
        string trueHolderReg;
        string trueLabel;
        int trueHoleLine = EmitLable ("true", trueHolderReg, trueLabel);

        string falseLabel;
        string falseHolderReg;
        int falseHoleLine = EmitLable ("false", falseHolderReg, falseLabel);

        
        string result = CreateNewLabel();
        string phi = EmitPhi(trueHolderReg, trueLabel, falseHolderReg, falseLabel);

        bufferPtr->emit("ret i1 " + phi);

        bufferPtr->bpatch(bufferPtr->makelist(make_pair(trueHoleLine, FIRST)), result);
        bufferPtr->bpatch(bufferPtr->makelist(make_pair(falseHoleLine, FIRST)), result);
        bufferPtr->bpatch(exp->GetTrueListToPatch(), trueLabel);
        bufferPtr->bpatch(exp->GetFalseListToPatch(), falseLabel);
    }

    void StoreNewValueToId(string idPointerReg, string valueReg)
    {
        bufferPtr->emit("store i32 " + valueReg + ", i32* " + idPointerReg);      
    }


};


#endif //EMITANDSHIT_H
