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
    Type f1 ("int", v1, v1 , false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2 ,v2, false);

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
    Type f1 ("int", v1, v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2, v2, false);

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

void check3 (){// check checkIfVarExist
    
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1, v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2, v2, false);

    SymbolTableElement R1 ("r1" ,a , 0);
    SymbolTableElement R2 ("r2" ,b , 1);
    SymbolTableElement R3 ("r3" ,c , 2);
    SymbolTableElement func1 ("func1" ,f1 , 3);
    SymbolTableElement func2 ("func2" ,f2 , 0);
    SymbolTableElement func3 ("func3" ,f2 , 4);


    ScopeStack ScopeStack1;

    ScopeStack stack;
    stack.PushNewScope(GLOBAL);
    ErrorType errorType;
    errorType = stack.checkIfVarExist("r1", "int");
    if (errorType == ERROR_UNDEF) {
        cout << "ok ";
    } else {
        cout << "***** error 1 checkIfVarExist" << endl ;
    }
    stack.printStack();

    stack.AddSymbolToCurrentScope(R1);
    stack.AddSymbolToCurrentScope(R2);

    errorType = stack.checkIfVarExist("r1", "bool");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 2 checkIfVarExist" << endl ;
    }
    errorType = stack.checkIfVarExist("r1", "string");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 3 checkIfVarExist" << endl ;
    }
    errorType = stack.checkIfVarExist("r1", "int");
    if (errorType == ERROR_UNDEF || errorType == ERROR_MISMATCH) {
        cout << "***** error 4 checkIfVarExist" << endl ;
    } else {
        cout << "ok ";
    }

    errorType = stack.checkIfVarExist("r2", "int");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 5 checkIfVarExist" << endl ;
    }

    errorType = stack.checkIfVarExist("r2", "string");
    if (errorType == ERROR_UNDEF || errorType == ERROR_MISMATCH) {
        cout << "***** error 6 checkIfVarExist" << endl ;
    } else {
        cout << "ok ";
    }
    stack.PushNewScope(REGULAR);

    errorType = stack.checkIfVarExist("r1", "bool");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 7 checkIfVarExist" << endl ;
    }
    errorType = stack.checkIfVarExist("r1", "string");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 8 checkIfVarExist" << endl ;
    }
    errorType = stack.checkIfVarExist("r1", "int");
    if (errorType == ERROR_UNDEF || errorType == ERROR_MISMATCH) {
        cout << "***** error 9 checkIfVarExist" << endl ;
    } else {
        cout << "ok ";
    }

    errorType = stack.checkIfVarExist("r2", "int");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 10 checkIfVarExist" << endl ;
    }

    errorType = stack.checkIfVarExist("r2", "string");
    if (errorType == ERROR_UNDEF || errorType == ERROR_MISMATCH) {
        cout << "***** error 11 checkIfVarExist" << endl ;
    } else {
        cout << "ok ";
    }

    stack.printStack();

    stack.AddSymbolToCurrentScope(R3);
    stack.AddSymbolToCurrentScope(func1);

    errorType = stack.checkIfVarExist("r3", "bool");
    if (errorType == ERROR_UNDEF || errorType == ERROR_MISMATCH) {
        cout << "***** error 12 checkIfVarExist" << endl ;
    } else {
        cout << "ok ";
    }

    errorType = stack.checkIfVarExist("func1", "string");
    if (errorType == ERROR_UNDEF) {
        cout << "ok ";
    } else {
        cout << "***** error 13 checkIfVarExist" << endl ;
    }

    errorType = stack.checkIfVarExist("func1", "bool");
    if (errorType == ERROR_UNDEF) {
        cout << "ok ";
    } else {
        cout << "***** error 14 checkIfVarExist" << endl ;
    }

    stack.PopScope();

    errorType = stack.checkIfVarExist("r3", "bool");
    if (errorType == ERROR_UNDEF) {
        cout << "ok ";
    } else {
        cout << "***** error 15 checkIfVarExist" << endl ;
    }

    stack.printStack();

    stack.AddSymbolToCurrentScope(R3);
    stack.AddSymbolToCurrentScope(func1);
    stack.PushNewScope(REGULAR);


    errorType = stack.checkIfVarExist("func1", "bool");
    if (errorType == ERROR_UNDEF) {
        cout << "ok ";
    } else {
        cout << "***** error 16 checkIfVarExist" << endl ;
    }

    errorType = stack.checkIfVarExist("r3", "bool");
    if (errorType == ERROR_UNDEF || errorType == ERROR_MISMATCH) {
        cout << "***** error 17 checkIfVarExist" << endl ;
    } else {
        cout << "ok ";
    }

    errorType = stack.checkIfVarExist("r3", "int");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 18 checkIfVarExist" << endl ;
    }

    stack.printStack();


    // check checkAfterCallIfExist

    // check checkForErrorBeforeAddSymbolToCurrentScope
}

void check4 () { //check checkAfterCallIfFuncExist
// ERROR_UNDEF_FUNC - calling a function that was not declared.
// ERROR_UNDEF_FUNC - using identifier that is not a func as a func.
// ERROR_PROTOTYPE_MISMATCH -no maching arguments 
// ERROR_MISMATCH - return type don't match 
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1,v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2,v2, false);

    SymbolTableElement R1 ("r1" ,a , 0);
    SymbolTableElement R2 ("r2" ,b , 1);
    SymbolTableElement R3 ("r3" ,c , 2);
    SymbolTableElement func1 ("func1" ,f1 , 3);
    SymbolTableElement func2 ("func2" ,f2 , 0);
    SymbolTableElement func3 ("func3" ,f2 , 4);

    ScopeStack stack;
    ErrorType errorType;
    stack.PushNewScope(GLOBAL);
        stack.PushNewScope(REGULAR);
            stack.AddSymbolToCurrentScope(R1);
            stack.AddSymbolToCurrentScope(R2);
                stack.PushNewScope(REGULAR);
                    stack.AddSymbolToCurrentScope(R3);
                    stack.AddSymbolToCurrentScope(func1);

    stack.printStack(); // r1 int , r2 string, r3 bool , func1: int <- int int int
    vector <string> arg1 = {"int", "int"};

    errorType = stack.checkAfterCallIfFuncExist("func1", v1);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 1 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func1", v1 ,"int");
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 2 checkAfterCallIfFuncExist" << endl ;
    }

    
    errorType = stack.checkAfterCallIfFuncExist("func1", v1 ,"string");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 3 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func1", arg1);
    if (errorType == ERROR_PROTOTYPE_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 4 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func5", arg1);
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 5 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("r1", arg1);
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 6 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("r5", arg1);
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 7 checkAfterCallIfFuncExist" << endl ;
    }


                stack.PopScope();
                    stack.AddSymbolToCurrentScope(func2);
                    stack.AddSymbolToCurrentScope(func3);

    stack.printStack(); // r1 int , r2 string, r3 bool , func2 bool <- bool string, func3  bool <- bool string

    errorType = stack.checkAfterCallIfFuncExist("func2", v2);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 8 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func3", v2);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 9 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func3", v2, "bool");
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 10 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func2", v2, "bool");
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 11 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func2", v2, "string");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 12 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func3", v2, "int");
    if (errorType == ERROR_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 13 checkAfterCallIfFuncExist" << endl ;
    }

     errorType = stack.checkAfterCallIfFuncExist("func3", v1, "int");
    if (errorType == ERROR_PROTOTYPE_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 14 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func2", v1, "bool");
    if (errorType == ERROR_PROTOTYPE_MISMATCH) {
        cout << "ok ";
    } else {
        cout << "***** error 15 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("r1", arg1);
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 16 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("r5", arg1);
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 17 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func1", v1);
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 18 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func1", v1 , "string");
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 19 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func1", v1 , "int");
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 20 checkAfterCallIfFuncExist" << endl ;
    }


                stack.PopScope();


    stack.printStack(); // r1 int , r2 string, r3 bool 
    errorType = stack.checkAfterCallIfFuncExist("func1", v1 , "int");
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 21 checkAfterCallIfFuncExist" << endl ;
    }

    errorType = stack.checkAfterCallIfFuncExist("func2", v2, "bool");
    if (errorType == ERROR_UNDEF_FUNC) {
        cout << "ok ";
    } else {
        cout << "***** error 22 checkAfterCallIfFuncExist" << endl ;
    }


}

void check5 () { //check checkForErrorBeforeAddSymbolToCurrentScope (simple)
    //ERROR_FUNC_NO_OVERRIDE - trying to run-over a function that is not override. 
    //ERROR_OVERRIDE_WITHOUT_DECLARATION - trying to override a function that is override without using override in the declaration
    // ERROR_DEF - val already exist 
    // ERROR_DEF - new func is identical to an existing func.  

    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1,v1, false);
    vector<string> v2 = {"bool", "string"};
    Type f2 ("bool", v2,v2, true);
    Type f3 ("bool", v2,v2, false);
    Type f4 ("bool", v2,v2, false);
    Type f5 ("bool", v1,v1,  false);
    Type f6 ("int", v2,v2, false);

    SymbolTableElement R1 ("r1" ,a , 0);
    SymbolTableElement R2 ("r2" ,b , 1);
    SymbolTableElement R3 ("r3" ,c , 2);
    SymbolTableElement R4 ("r4" ,c);
    SymbolTableElement R5 ("r5" ,c );
    SymbolTableElement func1 ("func1" ,f1 , 3);
    SymbolTableElement func2 ("func2" ,f2 , 0);
    SymbolTableElement func3 ("func3" ,f3 , 4);
    SymbolTableElement func4 ("func2" ,f4 , 4);
    SymbolTableElement func5 ("func2" ,f2 , 4);
    SymbolTableElement func6 ("func2" ,f4 , 4);

    ScopeStack stack;
    ErrorType errorType;
    stack.PushNewScope(GLOBAL);
        stack.PushNewScope(REGULAR);
            stack.AddSymbolToCurrentScope(R1);
            stack.AddSymbolToCurrentScope(R2);
                stack.PushNewScope(REGULAR);
                    stack.AddSymbolToCurrentScope(R3);
                    stack.AddSymbolToCurrentScope(func1);

    stack.printStack(); // r1 int , r2 string, r3 bool , func1: int <- int int int
    vector <string> arg1 = {"int", "int"};

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func5);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 1 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func6);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 2 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func2);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 3 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func3);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 4 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R4);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 5 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R5);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 6 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }


    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R1);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 7 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R2);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 8 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R3);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 9 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func1);
    if (errorType == ERROR_FUNC_NO_OVERRIDE) {
        cout << "ok ";
    } else {
        cout << "***** error 10 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }



                stack.PopScope();
                    stack.AddSymbolToCurrentScope(func2);
                    stack.AddSymbolToCurrentScope(func3);

                    stack.printStack(); // r1 int , r2 string, func2 (override) bool <- bool string, func3  bool <- bool string

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func1);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 11 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R1);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 12 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(R2);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 13 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func2);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 14 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func3);
    if (errorType == ERROR_FUNC_NO_OVERRIDE) {
        cout << "ok ";
    } else {
        cout << "***** error 15 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func4);
    if (errorType == ERROR_OVERRIDE_WITHOUT_DECLARATION) {
        cout << "ok ";
    } else {
        cout << "***** error 16 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }


                stack.PopScope();


                stack.printStack(); // r1 int , r2 string, r3 bool 

             
}

void check6 () { //check checkForErrorBeforeAddSymbolToCurrentScope (complicated)
    //ERROR_FUNC_NO_OVERRIDE - trying to run-over a function that is not override. 
    //ERROR_OVERRIDE_WITHOUT_DECLARATION - trying to override a function that is override without using override in the declaration
    // ERROR_DEF - val already exist 
    // ERROR_DEF - new func is identical to an existing func.  

     vector<string> v1 = {"int", "int", "int"};
    Type f1 ("int", v1, v1, true);
    vector<string> v2 = {"bool", "string"};
     vector<string> v3 = {"int", "int", "bool"};
    Type f3 ("bool", v1, v1, true);
    Type f2 ("bool", v2,v2, true);
    Type f4 ("bool", v3,v3, true);
    Type f5 ("int", v3,v3, true);
    Type f6 ("int", v2,v2, true);

    Type fail1 ("int", v1,v1, true);
    Type fail2 ("bool", v3,v3, true);
    Type fail3 ("bool", v1,v1, true);

    SymbolTableElement func1 ("func" ,f1);
    SymbolTableElement func2 ("func" ,f2);
    SymbolTableElement func3 ("func" ,f3);
    SymbolTableElement func4 ("func" ,f4);
    SymbolTableElement func5 ("func" ,f5);
    SymbolTableElement func6 ("func" ,f6);
    SymbolTableElement ff1 ("func" ,fail1);
    SymbolTableElement ff2 ("func" ,fail2);
    SymbolTableElement ff3  ("func" ,fail3);

    ScopeStack stack;
    ErrorType errorType;
    stack.PushNewScope(GLOBAL);
        stack.PushNewScope(REGULAR);
                stack.PushNewScope(REGULAR);
                    stack.AddSymbolToCurrentScope(func1);

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func2);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 1 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }
                    stack.AddSymbolToCurrentScope(func2);

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func3);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 2 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }
                    stack.AddSymbolToCurrentScope(func3);

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func4);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 3 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }
                    stack.AddSymbolToCurrentScope(func4);

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func5);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 4 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }
                    stack.AddSymbolToCurrentScope(func5);


    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(func6);
    if (errorType == NO_ERROR) {
        cout << "ok ";
    } else {
        cout << "***** error 5 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }
                    stack.AddSymbolToCurrentScope(func6);





    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(ff1);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 6 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(ff2);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 7 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

    errorType = stack.checkForErrorBeforeAddSymbolToCurrentScope(ff3);
    if (errorType == ERROR_DEF) {
        cout << "ok ";
    } else {
        cout << "***** error 8 checkForErrorBeforeAddSymbolToCurrentScope" << endl ;
    }

           
}

void check7 () { // check the mecanisef for offset with functions.
    Type a ("int");
    Type b ("string");
    Type c ("bool");
    vector<string> v1 = {"int", "int", "int"};
     vector<string> n1 = {"a", "b", "c"};
    Type f1 ("int", v1, n1, false);
    vector<string> v2 = {"bool", "string"};
    vector<string> n2 = {"d", "e"};
    Type f2 ("bool", v2,n2, true);
    Type f3 ("bool", v2,n2, false);
    Type f4 ("bool", v2,n2, false);
    Type f5 ("bool", v1,n1,  false);
    Type f6 ("int", v2,n2, false);

    SymbolTableElement r1 ("r1" ,a);
    SymbolTableElement r2 ("r2" ,b);
    SymbolTableElement r3 ("r3" ,c);
    SymbolTableElement r4 ("r4" ,c);
    SymbolTableElement r5 ("r5" ,c);


    ScopeStack stack;
    stack.PushNewScope(GLOBAL);
        stack.PushNewScope(REGULAR);
                    stack.AddSymbolToCurrentScope(r1);
                    stack.AddSymbolToCurrentScope(r2);
                    stack.AddSymbolToCurrentScope(r3);
        stack.PushNewScope(REGULAR);
stack.printStack();
stack.printOffsetStack();

        stack.PushNewScope(REGULAR);
stack.printOffsetStack();
                    stack.AddSymbolToCurrentScope(r4);
                    stack.AddSymbolToCurrentScope(r5);
        stack.PopScope();
stack.printStack();
stack.printOffsetStack();
                    stack.AddSymbolToCurrentScope(r4);
                    stack.AddSymbolToCurrentScope(r5);
stack.printStack();
stack.printOffsetStack();

} 

int main () {
    /*
    check1();
    check2();
    check3();
    check4();
    check5();
    check6();
    check7();
    */
   check7();
    return 0;
}