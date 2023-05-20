#include "databases.hpp"

/* ------------------------------- Type ------------------------------- */
string Type::ToString() {
    string type_to_string;
    if (IsFunc()) {
        type_to_string = "( Type is a func ";
        if (IsOverride()) { type_to_string += " with override, "; }
        type_to_string+= "with | return type - " + return_type + " | and arguments type: ";
        for (int i =0; i<arguments_types.size()-1; i++) {
            type_to_string+= arguments_types[i] + ", ";
        }
        type_to_string+= arguments_types[arguments_types.size()-1] + " |";
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
}
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
void ScopeStack::PushNewScope(ScopeType scope_type) {
    // meaning add a new scope with empty table.
    if (scopes_stack.empty()) {// this is the first scope
        offset_stack.push_front(0);
    } else {
        int cur_offset_in_stack = offset_stack.front();
        offset_stack.push_back(cur_offset_in_stack);
    }
    Scope new_scope (scope_type);
    scopes_stack.push_front(new_scope);

}
void ScopeStack::PopScope() {
    // meaning remove the last scope.
    scopes_stack.pop_front();
    offset_stack.pop_front();
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