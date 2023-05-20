//
// Created by ronyj on 20/05/2023.
//
#include "databases.hpp"
#include <iostream>

//TODO: this is for testing what I have already:

void check1 (){
    // checked Type, SymbolTableElement, SymbolTable
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2, false);

    SymbolTableElement R1 ("r1" ,a , 0);
    SymbolTableElement R2 ("r2" ,b , 1);
    SymbolTableElement R3 ("r3" ,c , 2);
    SymbolTableElement R4 ("func1" ,f1 , 3);
    SymbolTableElement R5 ("func2" ,f2 , 0);
    SymbolTableElement R6 ("func3" ,f2 , 4);

    SymbolTable myTable;
    myTable.AddToTable(R1);
    myTable.AddToTable(R2);
    myTable.AddToTable(R3);
    myTable.AddToTable(R4);
    myTable.AddToTable(R5);
    myTable.AddToTable(R6);
    myTable.AddToTable("func4", f1, 5);
    myTable.AddToTable("r4", c, 6);
    myTable.PrintTable();

    bool check;
    SymbolTableElement* se = myTable.SearchInTableByName("r1", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldnt find r1"<<endl;

    se = myTable.SearchInTableByName("r2", &check);
    if (check) {cout << se->ToString()<<endl; }
    else cout<< "-- error -- couldnt find r2"<<endl;

    se = myTable.SearchInTableByName("r3", &check);
    if (check) {cout << se->ToString()<<endl; }
    else cout<< "-- error -- couldnt find r3"<<endl;

    se = myTable.SearchInTableByName("func1", &check);
    if (check) {cout << se->ToString()<<endl; }
    else cout<< "-- error -- couldnt find func1"<<endl;

    se = myTable.SearchInTableByName("func2", &check);
    if (check) {cout << se->ToString()<<endl; }
    else cout<< "-- error -- couldnt find func2"<<endl;

    se = myTable.SearchInTableByName("func3", &check);
    if (check) {cout << se->ToString()<<endl; }
    else cout<< "-- error -- couldnt find func3"<<endl;

    se = myTable.SearchInTableByName("func33", &check);
    if (check) {cout << "-- error --"  << se->ToString()<<endl; }
    else cout<< "couldnt find func33"<<endl;

    se = myTable.SearchInTableByName("r15", &check);
    if (check) {cout << "-- error --" << se->ToString()<<endl; }
    else cout<< "couldnt find r15"<<endl;

}

void check2 (){
    //check scope, and scopeStack:
    ScopeStack stack;
    stack.printStack(); //expect stack is empty.
    stack.PushNewScope(GLOBAL);// first scope
    stack.printStack(); //expect stack is empty.

    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2, false);

    SymbolTableElement R1 ("r1" ,a , 0);
    SymbolTableElement R2 ("r2" ,b , 1);

    SymbolTableElement R3 ("r3" ,c , 2);
    SymbolTableElement R4 ("func1" ,f1 , 3);

    stack.AddSymbolToCurrentScope(R1);
    stack.printStack(); //expect stack is empty.
    stack.AddSymbolToCurrentScope(R2);
    stack.printStack(); //expect stack is empty.0

    bool check;
    SymbolTableElement* se = stack.SearchInAllScopesByName("r1", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r1"<<endl;

    se = stack.SearchInAllScopesByName("r1", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r1"<<endl;

    se = stack.SearchInAllScopesByName("r2", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r2"<<endl;

    se = stack.SearchInAllScopesByName("r3", &check);
    if (check) {cout << "-- error -- found r3" << se->ToString() <<endl; }
    else cout<< "couldn't find r3"<<endl;

    stack.PushNewScope(REGULAR);
    stack.AddSymbolToCurrentScope(R3);
    stack.printStack();
    stack.AddSymbolToCurrentScope(R4);
    stack.printStack();

    se = stack.SearchInAllScopesByName("r3", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r3"<<endl;

    se = stack.SearchInAllScopesByName("func1", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find func1"<<endl;

   se = stack.SearchInAllScopesByName("r1", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r1"<<endl;

    se = stack.SearchInAllScopesByName("r2", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r2"<<endl;


    se = stack.SearchInAllScopesByName("r5", &check);
    if (check) {cout << "-- error -- found r5" << se->ToString() <<endl; }
    else cout<< "couldn't find r5"<<endl;

    stack.PopScope();
    stack.printStack();

    se = stack.SearchInAllScopesByName("r3", &check);
    if (check) {cout << "-- error -- found r3" << se->ToString() <<endl; }
    else cout<< "couldn't find r3"<<endl;

    se = stack.SearchInAllScopesByName("r4", &check);
    if (check) {cout << "-- error -- found r4" << se->ToString() <<endl; }
    else cout<< "couldn't find r4"<<endl;

    se = stack.SearchInAllScopesByName("r1", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r1"<<endl;

    se = stack.SearchInAllScopesByName("r2", &check);
    if (check) {cout << se->ToString() <<endl; }
    else cout<< "-- error -- couldn't find r2"<<endl;


}
SymbolTable createSymbolTable1 (){
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2, false);

    SymbolTableElement R1 ("r1" ,a , 0);
    SymbolTableElement R2 ("r2" ,b , 1);
    SymbolTableElement R3 ("r3" ,c , 2);
    SymbolTableElement R4 ("func1" ,f1 , 3);
    SymbolTableElement R5 ("func2" ,f2 , 0);
    SymbolTableElement R6 ("func3" ,f2 , 4);

    SymbolTable myTable;
    myTable.AddToTable(R1);
    myTable.AddToTable(R2);
    myTable.AddToTable(R3);
    myTable.AddToTable(R4);
    myTable.AddToTable(R5);
    myTable.AddToTable(R6);
    myTable.AddToTable("func4", f1, 5);
    myTable.AddToTable("r4", c, 6);

    return myTable;

}
SymbolTable createSymbolTable2 (){
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2, false);

    SymbolTableElement R1 ("m1" ,a , 0);
    SymbolTableElement R2 ("m2" ,b , 1);

    SymbolTableElement R5 ("func12" ,f2 , 0);
    SymbolTableElement R6 ("func13" ,f2 , 4);

    SymbolTable myTable;
    myTable.AddToTable(R1);
    myTable.AddToTable(R2);
    myTable.AddToTable(R5);
    myTable.AddToTable(R6);

    return myTable;

}
SymbolTable createSymbolTable3 (){
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int", "bool", "bool", "bool"};
    Type f1 ("int", v1, false);

    SymbolTableElement R1 ("s1" ,a , 0);
    SymbolTableElement R2 ("s2" ,b , 1);
    SymbolTableElement R3 ("s3" ,c , 2);
    SymbolTableElement R4 ("func11" ,f1 , 3);

    SymbolTable myTable;
    myTable.AddToTable(R1);
    myTable.AddToTable(R2);
    myTable.AddToTable(R3);
    myTable.AddToTable(R4);

    return myTable;

}
int main () {
    check2();
    return 0;
}