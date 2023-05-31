#include "databases.hpp"
#include "hw3_output.hpp"
#include <vector>
#include <algorithm>
/* ------------------------------- Type ------------------------------- */
string TypeStruct::ToString() {
    string type_to_string;
    if (IsFunc()) {
        type_to_string = "( Type is a func ";
        if (IsOverride()) { type_to_string += " with override, "; }
        type_to_string+= "with | return type - " + return_type + " | and arguments type: ";
        for (int i =0; i<arguments_types.size()-1; i++) {
            type_to_string+=  arguments_types[i] + ", ";
        }
        type_to_string+= arguments_types[arguments_types.size()-1] + " |";
        type_to_string+= " ) ";
    }
    else {
        type_to_string = "( Type is ver of type - " + type + " ) ";
    }
    return type_to_string;
}
/* -------------------------- Symbol Table -------------------------- */

string SymbolTableElement::ToString() {
    string sym_entry_to_string;
    sym_entry_to_string = "{ name - " + name + " offset - " + std::to_string(offset) + " and type - " + type.ToString() +" } ";
    return sym_entry_to_string;
}
SymbolTableElement* SymbolTable::SearchInTableByName (string name, bool* found) {
    for (int index = 0; index < table_vector.size(); index++) {
        if (table_vector[index].GetName() == name) { //found
            *found  = true;
            return &table_vector[index];
        }
    }
    *found = false;
    return NOT_FOUND;
}
vector <SymbolTableElement*> SymbolTable::GetAllFuncOfName (string name) {
    vector <SymbolTableElement*> allFuncs;
    for (int index = 0; index < table_vector.size(); index++) {
        if (table_vector[index].GetName() == name) { //found
            allFuncs.push_back(&table_vector[index]);
        }
    }
    return allFuncs;
}
void SymbolTable::PrintTable () {
    cout << "printing table: " <<endl;
    for (int index = 0; index < table_vector.size(); index++) {
        cout << "   Index = " << index << " Entry - " << table_vector[index].ToString() << endl;
    }
    cout << endl;
}

string SymbolTable::ToString(){
    string table_to_string;
    table_to_string = "printing table: \n";
    for (int index = 0; index < table_vector.size(); index++) {
        table_to_string += " Entry - " + table_vector[index].ToString() + "\n";
    }
    table_to_string += "\n";
    return table_to_string;
}

/* --------------------------- stack of scopes --------------------------- */

string Scope::ScopeTypeStr() {
    switch (scope_type) {
        case WHILE_SCOPE:
            return "while";
            break;

        case IF_SCOPE:
            return "if";
            break;

        case ELSE_SCOPE:
            return "else";
            break;

        case FUNC_SCOPE:
            return "func";
            break;

        case REGULAR_SCOPE:
            return "regular";
            break;

        case GLOBAL_SCOPE:
            return "global";
            break;

        default:
            return "error no such scopeType";
    }
    return "error no such scopeType";
}

void Scope::PrintScope() {
    output::endScope();
    for(SymbolTableElement element : sym_table.table_vector)
    {
       if(element.IsFunc()){
           cout<<element.GetName()<<" "<< output::makeFunctionType(element.GetType().GetReturnType(),element.GetType().GetArgumentsTypes())<<" 0"<<endl;
       }
       else{
           output::printID(element.GetName(),element.GetOffset() ,element.GetType().GetTypeName());
       }
    }
}

SymbolTableElement* ScopeStack::SearchInAllScopesByName(string name, bool* found) {
    if (scopes_stack.empty()) {
        *found = false;
        return NOT_FOUND;
    }
    list<Scope>::iterator it;
    bool foundCur;
    SymbolTableElement* element;
    for (it = scopes_stack.begin(); it != scopes_stack.end(); it++) {
        element = it->sym_table.SearchInTableByName(name, &foundCur);
        if (foundCur) {
            *found = true;
            return element;
        }
    }
    *found = false;
    return NOT_FOUND;
}
vector <SymbolTableElement*> ScopeStack::GetAllFuncOfName (string name) {
    // NOTE: call this only after knowing there is at list one func with the same name
    vector <SymbolTableElement*> allFuncs;
    list<Scope>::iterator it;
    for (it = scopes_stack.begin(); it != scopes_stack.end(); it++) {
        vector <SymbolTableElement*> currentTableVec;
        currentTableVec = it->sym_table.GetAllFuncOfName(name);
        if (currentTableVec.size() > 0) {
            for (int i = 0; i < currentTableVec.size(); i++) {
                allFuncs.push_back (currentTableVec[i]);
            }
        }
    }
    return allFuncs;
}
void ScopeStack::printStack() {
    if (scopes_stack.empty()) {
        cout << "stack is empty";
        return;
    }
    cout << "Print stack with " << scopes_stack.size() <<" scopes :" <<endl;
    cout << "************************************************** " << endl;
    list<Scope>::iterator it;
    int index = 1;
    for (it = scopes_stack.begin(); it != scopes_stack.end(); it++) {
        cout <<"scope number "<< index << " :"<<endl;
        cout << it->ToString() << endl;
        cout << endl;
        index++;
    }
    cout << "************************************************** " << endl;
}
ErrorType ScopeStack::checkLegalOverride(SymbolTableElement element) {
    vector <SymbolTableElement*> allFuncs = GetAllFuncOfName(element.GetName());
    string newFuncRetType = element.GetType().GetReturnType();
    vector<string> newFuncArgTypes = element.GetType().GetArgumentsTypes();
    for (int i = 0; i< allFuncs.size(); i++) {
        if (newFuncRetType == allFuncs[i]->GetType().GetReturnType()) {
            if (newFuncArgTypes == allFuncs[i]->GetType().GetArgumentsTypes()) {
                return ERROR_DEF; // new func is identical to an existing func.
            }
        }
    }
    return NO_ERROR;
}


// for outside use:
/*
void ScopeStack::AddSymbolToCurrentScope(string name, Type type, int offset) {
    if (scopes_stack.empty()) { cout<< "ERROR trying to add to current scope when there is no scopes! "; return; }
    if (!type.IsFunc()) {
        int current_offset = offset_stack.front();
        offset_stack.pop_front();
        offset = current_offset;
        current_offset ++;
        offset_stack.push_front(current_offset);
    } else {
        offset = NA;
    }
    Scope* cur_scope = &scopes_stack.front();
    cur_scope->AddToScope(name, type, offset);
}*/

void ScopeStack::AddSymbolToCurrentScope(SymbolTableElement element) {
    if (scopes_stack.empty()) { cout<< "ERROR trying to add to current scope when there is no scopes! "; return; }
    if (!element.IsFunc()) {
        int current_offset = offset_stack.front();
        offset_stack.pop_front();
        element.SetOffset(current_offset);
        current_offset ++;
        offset_stack.push_front(current_offset);
    } else {
        element.SetOffset(NA);      
    }

    Scope* cur_scope = &scopes_stack.front(); // this is a copy!
    cur_scope->AddToScope(element);

}

void ScopeStack::AddArgumentToCurrentScope(SymbolTableElement element) {
    if (scopes_stack.empty()) { cout<< "ERROR trying to add to current scope when there is no scopes! "; return; }
    Scope* cur_scope = &scopes_stack.front(); // this is a copy!
    cur_scope->AddToScope(element);
}

void ScopeStack::printOffsetStack() {
    list<int>::iterator it;
    cout << "printing offset stack: ------- ";
    for (it = offset_stack.begin(); it != offset_stack.end(); it++) {
        cout << *it << endl;
    }
    cout << "--------------------------------- "<<endl;
}

void ScopeStack::PushNewScope(ScopeType scope_type, vector<pair<string,string>> types_names_arg_vector) {
    // meaning add a new scope with empty table.
    if (scopes_stack.empty()) {// this is the first scope
        offset_stack.push_front(0);
    } else {
        int cur_offset_in_stack = offset_stack.front();
        offset_stack.push_front(cur_offset_in_stack);
    }
    Scope new_scope (scope_type);
    scopes_stack.push_front(new_scope);
    if(scope_type == GLOBAL_SCOPE){
        //add library function print:string->void to global scope
        vector<string> print_args_type= {"STRING"};
        TypeStruct print_func("VOID", print_args_type);
        SymbolTableElement print_element("print",print_func);
        AddSymbolToCurrentScope(print_element);

        //add library function printi:int->void to global scope
        vector<string> printi_args_type= {"INT"};
        TypeStruct printi_func("VOID", printi_args_type);
        SymbolTableElement printi_element("printi",printi_func);
        AddSymbolToCurrentScope(printi_element);
    }

    if (scope_type == FUNC_SCOPE) { //TODO: Rony this was not checked
        // add the arguments to the stack as vars:
        vector<string> argumentsType;
        vector<string> argumentsNames;
        for (auto pair :types_names_arg_vector){
            argumentsType.push_back(pair.first);
            argumentsNames.push_back(pair.second);
        }
        for (int i = 0; i<argumentsType.size(); i++) {
            TypeStruct curType (argumentsType[i]);
            int offset = -1*(i+1);
            SymbolTableElement curElem (argumentsNames[i], curType, offset);
            AddArgumentToCurrentScope(curElem);
        }
    }
}

void ScopeStack::PopScope() {
    // meaning remove the last scope.
    if(!scopes_stack.empty()) {
        scopes_stack.pop_front();
        offset_stack.pop_front();
    }
}
// new :
ErrorType ScopeStack::checkForErrorBeforeAddSymbolToCurrentScope(SymbolTableElement element) {
    bool found;
    if (element.IsFunc()) {
        SymbolTableElement* onefuncInStack;
        onefuncInStack = SearchInAllScopesByName (element.GetName(), &found);
        if (found) {
            if (element.GetType().IsOverride()) { // new func is override
                // the added func is override, and there is a function with the same name in the stack
                if (!onefuncInStack->GetType().IsOverride()) { // old func is NOT override
                    return ERROR_FUNC_NO_OVERRIDE; // trying to run-over a function the is not override.
                } else { // old func is override
                    return checkLegalOverride(element);
                }
            } else {  // new func is NOT override
                if (!onefuncInStack->GetType().IsOverride()) {  // old func is NOT override
                    return ERROR_FUNC_NO_OVERRIDE; // trying to run-over a function the is not override.
                }
                else { // old func is override
                    return ERROR_OVERRIDE_WITHOUT_DECLARATION; // trying to override a function that is override without using override in the declaration
                }
            }
        }
    } else { // not a func just a var
        SymbolTableElement* oldVar;
        oldVar = SearchInAllScopesByName (element.GetName(), &found);
        if (found) {
            return ERROR_DEF; // val already exist 
        }
    }

    return NO_ERROR;
}
ErrorType ScopeStack::checkAfterCallIfFuncExist(string expectedFuncName, vector<pair<string,pair<string,bool>>> types_names_isId_arg_vector, string expectedReturnType ) {
    bool found;
    SymbolTableElement* func;
    vector<string> funcArgument;
    vector<string> expectedArgumentsTypes;
    vector <pair<string,bool>> expectedArgumentsNames;
    for (auto pair :types_names_isId_arg_vector){
        expectedArgumentsTypes.push_back(pair.first);
        expectedArgumentsNames.push_back(pair.second);
    }
    
    func = SearchInAllScopesByName (expectedFuncName, &found);
    if (!found || func == nullptr) {
        return ERROR_UNDEF_FUNC; // calling a function that was not declared.
    }
    if (!func->IsFunc()) {
        return ERROR_UNDEF_FUNC; // using identifier that is not a func as a func.
    }

    // check if the the vars that are id exist:
    for (auto var: expectedArgumentsNames) {
        if(var.second) {
            ErrorType error = checkIfVarExist(var.first);
            if (error != NO_ERROR) {
                return error;
            }
        }
    }

    // for arguments that are ID, need to find the types:
    int i = 0;
    for (auto pair : expectedArgumentsNames) {
        if (pair.second) { //isID
            bool found;
            SymbolTableElement* curElem = SearchInAllScopesByName(pair.first, &found);
            if (!found) {
                return ERROR_UNDEF;
            }
            string curTypeName = curElem->GetType().GetTypeName();
            expectedArgumentsTypes[i] = curTypeName;
        }
        i++;
    }

    reverse(expectedArgumentsTypes.begin(), expectedArgumentsTypes.end()); // OREN this is created in a reverse order in the expList 
    funcArgument = func->GetType().GetCopyOfArgumentsTypes();   
    if (funcArgument != expectedArgumentsTypes) {
        return ERROR_PROTOTYPE_MISMATCH; // no maching arguments 
    }
    if (expectedReturnType != "dont care") {
        if (expectedReturnType != func->GetType().GetReturnType()) {
            return ERROR_MISMATCH; //type don't match
        }
    }

    return NO_ERROR;
}
ErrorType ScopeStack::checkIfVarExist(string expectedVarName, string expectedVarType) {
    bool found;
    SymbolTableElement* var;
    var = SearchInAllScopesByName (expectedVarName, &found);
    if(!found || var == nullptr) {
        return ERROR_UNDEF; // using a var that was not declared
    }
    if (var->IsFunc()) {
        return ERROR_UNDEF; // using a var that is not a var (a function);
    }
    if (expectedVarType != "dont care") {
        if (expectedVarType != var->GetType().GetTypeName()
        && !(expectedVarType =="int" && var->GetType().GetTypeName()=="byte")) {
            return ERROR_MISMATCH; //type don't match
        }
    }
    return NO_ERROR;
}

void ScopeStack::PushNewScope(string scope_type,vector<pair<string,string>> types_names_arg_vector) {
    if(scope_type == "GLOBAL") ScopeStack::PushNewScope(GLOBAL_SCOPE);
    else if(scope_type == "WHILE") ScopeStack::PushNewScope(WHILE_SCOPE);
    else if(scope_type == "IF") ScopeStack::PushNewScope(IF_SCOPE);
    else if(scope_type == "ELSE") ScopeStack::PushNewScope(ELSE_SCOPE);
    else if(scope_type == "FUNC") ScopeStack::PushNewScope(FUNC_SCOPE,types_names_arg_vector);
    else if(scope_type == "REGULAR") ScopeStack::PushNewScope(REGULAR_SCOPE);
}

void ScopeStack::PrintCurrentScope() {
    //this->printStack();
    scopes_stack.front().PrintScope();
}

ErrorType ScopeStack::checkIfAssignedTypesAreCompatible(string expectedVarType, string givenVarType){
    if (expectedVarType != givenVarType
        && !(expectedVarType =="int" && givenVarType=="byte")) {
        return ERROR_MISMATCH; //type don't match
    }
    return NO_ERROR;
}

