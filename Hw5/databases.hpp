#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "types.h"
#include "Register.h"

using namespace std;

typedef enum {WHILE_SCOPE, IF_SCOPE, ELSE_SCOPE, FUNC_SCOPE, REGULAR_SCOPE, GLOBAL_SCOPE} ScopeType;

typedef enum {NO_ERROR, ERROR_LEX, ERROR_SYN,
              ERROR_UNDEF, ERROR_UNDEF_FUNC, ERROR_DEF,
              ERROR_PROTOTYPE_MISMATCH, ERROR_MISMATCH,
              ERROR_UNEXPECTED_BREAK, ERROR_UNEXPECTED_CONTINUE,
              ERROR_MAIN_MISSING, ERROR_BYTE_TOO_LARGE,
              ERROR_FUNC_NO_OVERRIDE, ERROR_OVERRIDE_WITHOUT_DECLARATION,
              ERROR_AMBIGUOUS_CALL, ERROR_MAIN_OVERRIDE} ErrorType;

#define NOT_FOUND nullptr
#define NA -99999
#define UNDEF -88888
/* ------------------------------- Type ------------------------------- */

class TypeStruct {
protected:
    string type;
    //function related:
    bool isOverride; // true when the function has override in its declaration
    string return_type;
    vector<string> arguments_types;
    vector<string> arguments_regs;
    bool isNoArguments;
    Register reg;


public:
    // constructor for non-function :
    TypeStruct(string type) : type(type), return_type("NULL") , isOverride(false){}

    // constructor for a function :
    TypeStruct(string return_type, vector<string> arguments_types, bool isOverride = false,vector<string> arguments_regs =vector<string>() ) :
            type("NULL"), return_type(return_type), arguments_types(arguments_types), isOverride(isOverride),arguments_regs(arguments_regs){ isNoArguments = arguments_types.empty();}
    virtual ~TypeStruct() {};
    bool IsFunc() { return (return_type != "NULL"); }
    bool IsOverride() { return isOverride; }
    string GetTypeName() { return type; }
    string GetReturnType() { return return_type; }
    vector<string>& GetArgumentsTypes() { return arguments_types; }
    vector<string> GetCopyOfArgumentsTypes() { return arguments_types; }
    string ToString();
    bool IsNoArguments() { return isNoArguments; }
    void SetReg(int regNumber) { reg.SetRegister(regNumber);}
    void SetReg(string regValue) { reg.SetRegister(regValue);}
    void SetArgumentsRegs (vector <string> new_arg_reg_vec) { arguments_regs = new_arg_reg_vec; }
    string GetReg();
};

/* --------------------------- Symbol Table --------------------------- */
class SymbolTableElement {
protected:
    string name;
    TypeStruct type;
    int offset;

public:
    SymbolTableElement(string name, TypeStruct type, int offset) : name(name), type(type), offset(offset) {}
    SymbolTableElement(string name, TypeStruct type) : name(name), type(type), offset(UNDEF) {}

    string GetName() {return name; }
    TypeStruct GetType() { return type; }
    int GetOffset() { return offset; }
    void SetOffset(int new_offset) { offset = new_offset; }
    bool IsFunc () { return type.IsFunc(); }
    string ToString();
    void SetReg(int reg_number) { type.SetReg(reg_number);}
    void SetReg(string regValue) { type.SetReg(regValue);}

    string GetReg(){return type.GetReg();}
};
class SymbolTable {
    //OREN - we are not using pointers cuz we are lazy, just like we did in OS last semester...
public:
    vector<SymbolTableElement> table_vector;
    SymbolTable() = default;
    void AddToTable (SymbolTableElement element) { table_vector.push_back(element); }
    void AddToTable (string name, TypeStruct type, int offset) { SymbolTableElement element (name, type, offset); AddToTable (element); }
    void PrintTable ();// for debug
    string ToString();
    SymbolTableElement* SearchInTableByName (string name, bool* found); // must check found to see if the return is valid!!
    vector <SymbolTableElement*> GetAllFuncOfName (string name); //might return empty vac


};
/* -------------------------------- scope --------------------------------- */
class Scope {
protected:
    ScopeType scope_type;
    string funcReturnType; // relevant only to FUNC scoop.
public:
    SymbolTable sym_table;
    Scope (SymbolTable sym_table, ScopeType scope_type) : sym_table(sym_table), scope_type(scope_type) {}
    Scope (ScopeType scope_type) : scope_type(scope_type) {} 
    ScopeType GetType() { return scope_type; }
    void AddToScope (SymbolTableElement element) { sym_table.AddToTable(element); }
    void AddToScope(string name, TypeStruct type, int offset) { SymbolTableElement element (name, type, offset); sym_table.AddToTable(element); }
    SymbolTableElement* SearchInScopeByName (string name, bool* found) { return sym_table.SearchInTableByName(name, found); } // must check found to see if the return is valid!!
    string ScopeTypeStr();
    string ToString () { return "[ scope : type - " + ScopeTypeStr() + ", table :\n" + sym_table.ToString(); + " ]"; }
    void PrintScope();
    void SetFuncReturnType(string returnType) { funcReturnType = returnType; }
    string GetFuncReturnType() { return funcReturnType; }
};
/* --------------------------- stack of scopes --------------------------- */
class ScopeStack {

public:
    list<Scope> scopes_stack;
    list<int> offset_stack;
    ScopeStack(){}
    SymbolTableElement* SearchInAllScopesByName(string name, bool* found);
    vector <SymbolTableElement*> GetAllFuncOfName (string name); //note to use this after knowing there is at list one func with this name in the stack.
    ErrorType checkLegalOverride(SymbolTableElement element);
    void printOffsetStack();
    void PrintCurrentScope();

    // for outside use:
    void AddSymbolToCurrentScope(SymbolTableElement element);
    void AddSymbolToCurrentScope(string name, TypeStruct type, int offset = UNDEF) { SymbolTableElement element (name, type, offset); return AddSymbolToCurrentScope (element); }
    void PushNewScope(ScopeType scope_type, string returnType, vector<pair<string,string>> types_names_arg_vector = vector<pair<string,string>>());
    void PushNewScope(string scope_type, string returnType, vector<pair<string,string>> types_names_arg_vector = vector<pair<string,string>>());
    void AddArgumentToCurrentScope(SymbolTableElement element);
    void PopScope();
    ErrorType checkForErrorBeforeAddSymbolToCurrentScope(string name, TypeStruct type, int offset = UNDEF) { SymbolTableElement element (name, type, offset); return checkForErrorBeforeAddSymbolToCurrentScope (element); }
    ErrorType checkForErrorBeforeAddSymbolToCurrentScope(SymbolTableElement element); //for both func or variable
    ErrorType checkAfterCallIfFuncExist(string expectedFuncName, vector<pair<string,pair<string,bool>>> types_names_isId_arg_vector ,string expectedReturnType = "dont care" );// Oren, if you don't care to check the return type dont add it in the arguments
    ErrorType CheckCallArgumentToFunc(SymbolTableElement* func, vector <pair<string,bool>> expectedArgumentsNames , vector<string> expectedArgumentsTypes, string* returnTypeOfFuncThatMatch ,string expectedReturnType);
    ErrorType checkIfAllArgumentsExist(vector<pair<string,pair<string,bool>>> types_names_isId_arg_vector , string* var_error_name );  
    ErrorType checkIfVarExist(string expectedVarName, string expectedVarType = "dont care"); //Oren, if you don't care to check the type don't add it in the arguments
    ErrorType checkIfAssignedTypesAreCompatible(string expectedVarName, string expectedVarType);
    string GetFunctionReturnType(string funcName,vector<pair<string,pair<string,bool>>> types_names_isId_arg_vector);
    void printStack(); //for debug
    bool CurScopeRetTypeEquals (string expected_type);
    bool IsCurrentScopeWhile();
};


//TODO: oren didnt use: void errorMainOverride(int yylineno)?? when should i;