#include <string>
#include <vector>
#include <list>
#include <iostream>
//#include "hw3_output.hpp"
using namespace std;
typedef enum {WHILE, IF, ELSE, FUNC, REGULAR, GLOBAL} ScopeType;
#define NOT_FOUND nullptr
#define NA -99999
/* ------------------------------- Type ------------------------------- */
class Type {
protected:
    string type_name;
    //function related:
    bool isOverride; // true when the function has override in its declaration
    string return_type;
    vector<string> arguments_types;

public:
    // constructor for non-function :
    Type(string type_name) : type_name(type_name), return_type("NULL") , isOverride(false){}

    // constructor for a function :
    Type(string return_type, vector<string> arguments_types, bool isOverride) :
            type_name("NULL"), return_type(return_type), arguments_types(arguments_types) , isOverride(isOverride) {}

    bool IsFunc() { return (return_type != "NULL"); }
    bool IsOverride() { return isOverride; }
    string GetTypeName() { return type_name; }
    string GetReturnType() { return return_type; }
    vector<string> GetArgumentsTypes() { return arguments_types; }
    string ToString();
};
/* --------------------------- Symbol Table --------------------------- */
class SymbolTableElement {
protected:
    string name;
    Type type;
    int offset;

public:
    SymbolTableElement(string name, Type type, int offset) : name(name), type(type), offset(offset) {}

    string GetName() {return name; }
    Type GetType() { return type; }
    int GetOffset() { return offset; }
    void SetOffset(int new_offset) { offset = new_offset; }
    bool IsFunc () { return type.IsFunc(); }
    string ToString();
};

class SymbolTable {
    //OREN - we are not using pointers cuz we are lazy, just like we did in OS last semester...
public:
    vector<SymbolTableElement> table_vector;
    SymbolTable() = default;
    void AddToTable (SymbolTableElement element) { table_vector.push_back(element); }
    void AddToTable (string name, Type type, int offset) { SymbolTableElement element (name, type, offset); AddToTable (element); }
    void PrintTable ();// for debug
    string ToString();
    SymbolTableElement* SearchInTableByName (string name, bool* found); // must check found to see if the return is valid!!
    //TODO: OREN we will have to think of how to implement the search when its an override function, I leave it open for now
};
/* -------------------------------- scope --------------------------------- */
class Scope {
protected:
    ScopeType scope_type;
public:
    SymbolTable sym_table;
    Scope (SymbolTable sym_table, ScopeType scope_type) : sym_table(sym_table), scope_type(scope_type) {}
    Scope (ScopeType scope_type) : scope_type(scope_type) {} //TODO: RONY do I need to init the sym_table somehow?
    ScopeType GetType() { return scope_type; }
    void AddToScope (SymbolTableElement element) { sym_table.AddToTable(element); }
    void AddToScope(string name, Type type, int offset) { SymbolTableElement element (name, type, offset); sym_table.AddToTable(element); }
    SymbolTableElement* SearchInScopeByName (string name, bool* found) { return sym_table.SearchInTableByName(name, found); } // must check found to see if the return is valid!!
    string ScopeTypeStr();
    string ToString () { return "[ scope : type - " + ScopeTypeStr() + ", table :\n" + sym_table.ToString(); + " ]"; }

};
/* --------------------------- stack of scopes --------------------------- */
class ScopeStack {

public:
    list<Scope> scopes_stack;
    list<int> offset_stack; //TODO: OREN I made sure to update the offset like in the tutorial, and update the var/func BUT I didnt added the aruments as negative offset yet. didnt know when and how it should be done
    ScopeStack() {} //TODO: OREN for now there is a need to OpenNewScope for the first scope
    void AddSymbolToCurrentScope(SymbolTableElement element);
    void AddSymbolToCurrentScope(string name, Type type, int offset = -1);
    void PushNewScope(ScopeType scope_type);
    void PopScope();
    SymbolTableElement* SearchInAllScopesByName(string name, bool* found);
    void printStack(); //for debug
};