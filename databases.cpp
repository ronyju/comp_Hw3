#include "databases.hpp"

/* ------------------------------- Type ------------------------------- */
string Type::ToString() {
    string type_to_string;
    if (IsFunc()) {
        type_to_string = "( Type is a func ";
        if (IsOverride()) { type_to_string += " with override, "; }
        type_to_string+= "with | return type - " + return_type + " | and arguments type: ";
        for (int i =0; i<arguments_types.size()-1; i++) {
            type_to_string+= arguments_names[i] + " : " + arguments_types[i] + ", ";
        }
        type_to_string+=  arguments_names[arguments_types.size()-1] + " : " + arguments_types[arguments_types.size()-1] + " |";
        type_to_string+= " ) ";
    }
    else {
        type_to_string = "( Type is ver of type - " + type_name + " ) ";
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
        case WHILE:
            return "while";
            break;

        case IF:
            return "if";
            break;

        case ELSE:
            return "else";
            break;

        case FUNC:
            return "func";
            break;

        case REGULAR:
            return "regular";
            break;

        case GLOBAL:
            return "global";
            break;

        default:
            return "error no such scopeType";
    }
    return "error no such scopeType";
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

    if (element.IsFunc())   {
        // add the aruments to the stack as vars:
        vector<string> argumentsType = element.GetType().GetArgumentsTypes();
        vector<string> argumentsNames = element.GetType().GetArgumentsNames();
        for (int i = 0; i<argumentsType.size(); i++) {
            Type curType (argumentsType[i]);
            int offset = -1*(i+1);
            SymbolTableElement curElem (argumentsNames[i], curType, offset);
            cur_scope->AddToScope(curElem);
        }
    }
}

void ScopeStack::printOffsetStack() {
    list<int>::iterator it;
    cout << "printing offset stack: ------- ";
    for (it = offset_stack.begin(); it != offset_stack.end(); it++) {
        cout << *it << endl;
    }
    cout << "--------------------------------- "<<endl;
}

void ScopeStack::PushNewScope(ScopeType scope_type) {
    // meaning add a new scope with empty table.
    if (scopes_stack.empty()) {// this is the first scope
        offset_stack.push_front(0);
    } else {
        int cur_offset_in_stack = offset_stack.front();
        offset_stack.push_front(cur_offset_in_stack);
    }
    Scope new_scope (scope_type);
    scopes_stack.push_front(new_scope);

}
void ScopeStack::PopScope() {
    // meaning remove the last scope.
    scopes_stack.pop_front();
    offset_stack.pop_front();
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
ErrorType ScopeStack::checkAfterCallIfFuncExist(string expectedFuncName, vector<string> expectedArgumentsTypes, string expectedReturnType ) {
    bool found;
    SymbolTableElement* func;
    func = SearchInAllScopesByName (expectedFuncName, &found);
    if (!found || func == nullptr) {
        return ERROR_UNDEF_FUNC; // calling a function that was not declared.
    }
    if (!func->IsFunc()) {
        return ERROR_UNDEF_FUNC; // using identifier that is not a func as a func.
    }
    if (func->GetType().GetArgumentsTypes() != expectedArgumentsTypes) {
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
        if (expectedVarType != var->GetType().GetTypeName()) {
            return ERROR_MISMATCH; //type don't match
        }
    }
    return NO_ERROR;
}

