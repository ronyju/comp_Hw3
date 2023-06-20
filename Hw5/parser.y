//declerations
%{
    #include <unordered_set>
    #include <iostream>
    #include "hw3_output.hpp"
    //#include "databases.hpp"
    #include "types.h"
    #include "EmitAndShit.h"
    extern int yylex();
    extern int yylineno;
    extern char *yytext;
    bool HasMain;
    string funcRetType;
    ScopeStack scopStack;
    void popScope();
    void yyerror(const char*);
    string functionReturnType;
	void addElementToScope(string type, string name, string valueType, string valueName , int yylineno);
    void addFuncToScope(bool isOverride,string returnType,string funcName,int yylineno);
    vector<pair<string,string>> types_names_arg_vector;
    vector<pair<string,pair<string,bool>>> types_names_isId_arg_vector;
    void handleErrorCode(ErrorType error, int lineno, const string& name, const string& value);
    Node* handeleMultiplacativeAndAdditiveORRelop(Node* exp1,Node* exp2 ,int yylineno, int isRelop = false);
    Node* handleBoolean (Node* exp1,Node* exp2 ,int yylineno);
    void checkAfterCallIfFuncExist (string func_name);
    string GetFunctionReturnTypeByName (string name);
    string GetFunctionReturnTypeByNameAndArgs(string name,Node* givenArgs);
    void checkIfConversionLegit(string new_type, string old_type, string name, int yylineno);
    void checkRet(string valType, string valName, int yylino);
    void checkIfExpIsBool();
	Node* last_exp;
	void IsCurrentScopeWhile(string type);
    bool areAllArgsNamesUnique(const vector<string>& names, string funcName, string* dupName);   
    void assignNewValToExistName(string name,Node* givenNodeVal, int lineno);
    void checkByteSize(string byteValString);
    #define MAX_BYTE 255

    ///////////////////////////// HW5 //////////////////////////
    EmitAndShit midCode;
    vector<pair<int,BranchLabelIndex>> makelist (pair<int,BranchLabelIndex> item);

//  for debug remove before submiting :
//    #define YYERROR_VERBOSE 1 
//    #define YYDEBUG 1
%}

// The roles here from the tutorial:
// left tokens (reduce rather than shift)
  
// right tokens (shift rather than reduce)
  // example: a^b^c ,  b : a ? b

// the last role is the preferable, that will take in acount only %left, %right and %nonassoc. It wont include %token


%token OVERRIDE

%token TRUE
%token FALSE

%token RETURN
%token IF
%token ELSE

%token WHILE
%token BREAK

%token CONTINUE

%token SC
%token COMMA

%token ID
%token NUM
%token STRING

%token VOID
%token INT
%token BYTE
%token B
%token BOOL

%right ASSIGN                   //Associativity - right-to-left
%left OR                        //Associativity - left to right 
%left AND                       //Associativity - left to right 

//%left       RELOP //relop was divided to relational and equality, relational is in higher priority than equality 
%left       EQUALITY            //Associativity - left to right 
%left       RELATIONAL          //Associativity - left to right


//%left       BINOP  //binop was divided to additive and multiplicative, multiplicative is in higher priority than additive
%left       ADDITIVE            //Associativity - left to right 
%left       MULTIPLICATIVE      //Associativity - left to right 


%right NOT     // 'NOT' must be here - this was checked  

%left LPAREN 
%left RPAREN
%left LBRACE
%left RBRACE


%%
//rules
Program     : M_GLOBAL_SCOPE Funcs                                                              {if (!HasMain) {handleErrorCode(ERROR_MAIN_MISSING, yylineno, "NONE" ,"needToAddValueForByteError");} popScope(); }  

Funcs	    : /*the_empty_word*/                                                    
		    | FuncDecl Funcs                                                      

FuncDecl    : OverRide RetType ID LPAREN Formals RPAREN { if ($3->GetValue() == "main" && $2->GetType()=="VOID" && types_names_arg_vector.empty()) { HasMain = true; } 
                addFuncToScope($1->GetType() == "override",$2->GetType(),$3->GetValue(),yylineno); functionReturnType = $2->GetType();} M_FUNC_SCOPE LBRACE 
                { midCode.FuncDeclareEmitToBuff($2->GetType(),$3->GetValue(),types_names_arg_vector);  types_names_arg_vector.clear();} Statements RBRACE {popScope(); midCode.AddRetVoidIfNeeded($11->GetHasRet());  midCode.EmitEndOfFunc();} //TODO: how to add return (ret void)

OverRide    : /*the_empty_word*/ {$$ = new Node("NONE","NONE",yylineno);}                                                  
		    | OVERRIDE           {$$ = $1;}                                                      

RetType     : Type               {$$ = $1; funcRetType = $1->GetType();}                                           
	        | VOID               {$$ = $1; funcRetType = "VOID";}                                                

Formals     : /*the_empty_word*/                                                  
		    | FormalsList                                                          

FormalsList	: FormalDecl                                                          
			| FormalDecl COMMA FormalsList                                        

FormalDecl	: Type ID             {types_names_arg_vector.push_back(make_pair($1->GetType(),$2->GetValue()));}                                               
                                                

// deviding the statment like done in the wikipedia page (https://en.wikipedia.org/wiki/Dangling_else)
// Oren I also added the while as an IF, cuz the compiler showed a S/R conflict with while and inside/after it IF ELSE

Statements	: Statement  {$$ = $1;}                                                          
			| Statements Statement {$$ = $1;} //{ midCode.Patch($2->GetLabel(),$1->GetNextListToPatch()); }// patch the nextList of Statements (the end of the first block) to the begining of the second block

Statement	: OpenIfWhileStatement MMMMMMMMMMMMMM {$$ = $1; midCode.Patch($2->GetLabel(),$1->GetNextListToPatch());} 
            | ClosedIfWhileStatment {$$ = $1;}



OpenIfWhileStatement    : IF M_IF_SCOPE LPAREN Exp RPAREN M_CHECK_BOOL Statement                                            				{popScope();}                                        
                        | IF M_IF_SCOPE LPAREN Exp RPAREN M_CHECK_BOOL ClosedIfWhileStatment M_POP_IF_SCOPE ELSE M_ELSE_SCOPE OpenIfWhileStatement      	{popScope();}
                        | WHILE NNNNNNNNNNNNNN M_WHILE_SCOPE LPAREN MMMMMMMMMMMMMM Exp RPAREN M_CHECK_BOOL MMMMMMMMMMMMMM OpenIfWhileStatement NNNNNNNNNNNNNN              							{popScope();}                       


ClosedIfWhileStatment   : NotIfWhileStatement {$$ = $1;}
                        | ClosedYesIfWhileStatment MMMMMMMMMMMMMM {$$ = $1; midCode.Patch($2->GetLabel(),$1->GetNextListToPatch());}

ClosedYesIfWhileStatment : IF M_IF_SCOPE LPAREN Exp RPAREN M_CHECK_BOOL ClosedIfWhileStatment M_POP_IF_SCOPE ELSE M_ELSE_SCOPE ClosedIfWhileStatment     	{popScope();}
                         | WHILE NNNNNNNNNNNNNN M_WHILE_SCOPE LPAREN MMMMMMMMMMMMMM Exp RPAREN M_CHECK_BOOL MMMMMMMMMMMMMM ClosedIfWhileStatment  NNNNNNNNNNNNNN {popScope();                                                                                                                                    
                                                                                                                                    $10->MergeToNextList($11->GetNextListToPatch()); // add the N address to the next list of the statment 
                                                                                                                                    midCode.Patch($5->GetLabel(),$2->GetNextListToPatch()); //patching the start of the block to the first entry - WHYYYY??!??!?!?! FFS  cuz LLVM says so 
                                                                                                                                    midCode.Patch($5->GetLabel(),$10->GetNextListToPatch()); //patching the end of the while loop to the boolean check 
                                                                                                                                    midCode.Patch($9->GetLabel(),$6->GetTrueListToPatch()); // patching the true list of the boolean check to the body of the while loop
                                                                                                                                    $$->MergeToNextList($6->GetFalseListToPatch());} // ClosedIfWhileStatment.next is now also respondible to patch the false list of the boolean check                       

NotIfWhileStatement     : LBRACE M_REGULAR_SCOPE Statements RBRACE          {popScope();}                                          
                        | Type ID SC                 	                    {addElementToScope($1->GetType(), $2->GetValue() , "NO_VALUE", "NO_VALUE" ,yylineno); 
                                                                                string newIdReg = midCode.IdDeclareEmitToBuff($1->GetType(), $2->GetValue()); //TODO: make sure to update the ID reg in the scope
                                                                                $2->SetCurrentRegister(newIdReg);
                                                                                $2->SetCurrentRegType("i32");
                                                                            }                       
                        | Type ID ASSIGN Exp SC      		                {addElementToScope($1->GetType(), $2->GetValue() , $4->GetType() , $4->GetValue() ,yylineno); //TODO: make sure to update the ID reg in the scope
                                                                                string newIdReg = midCode.IdAssignEmitToBuff($1->GetType(),$2->GetValue(),$4->GetValue() ,$4->GetType(), $4->GetCurrentRegister(), $4->GetCurrentRegType());
                                                                                $2->SetCurrentRegister(newIdReg);
                                                                            }                                             
                        | ID ASSIGN Exp SC           		                {assignNewValToExistName($1->GetValue(),$3 , yylineno);}  //TODO: make sure to update the ID reg in the scope                                        
                        | Call SC                                           {$$ = $1;}                  
                        | RETURN SC                                         {$$ = $1; checkRet("VOID" ,"", yylineno); 
                                                                                midCode.AddRetVoidIfNeeded(true); $$->SetHasRetToTrue();} //  emiting "void ret" and update it was done                                                           
                        | RETURN Exp SC                                     {$$ = $1; checkRet($2->GetType(), $2->GetValue() , yylineno);
                                                                                $$->SetHasRetToTrue(); } //TODO: emit ret this
                        | BREAK SC                                          {IsCurrentScopeWhile($1->GetType());}	                                                          
                        | CONTINUE SC                                       {IsCurrentScopeWhile($1->GetType());}	                                                   


Call 	    : ID LPAREN ExpList RPAREN                                      {checkAfterCallIfFuncExist($1->GetValue()); $$ = new Node (GetFunctionReturnTypeByName($1->GetValue()),$1->GetValue(), yylineno);}   //TODO: make sure to update the ID reg in the scope                                   
            | ID LPAREN RPAREN                                              {checkAfterCallIfFuncExist($1->GetValue()); $$ = new Node (GetFunctionReturnTypeByName($1->GetValue()),$1->GetValue(), yylineno);}    //TODO: make sure to update the ID reg in the scope               

ExpList	    : Exp	                                                        {types_names_isId_arg_vector.push_back(make_pair($1->GetType(), make_pair($1->GetValue(), $1->GetType()=="ID")));}                                     
		    | Exp COMMA ExpList                                             {types_names_isId_arg_vector.push_back(make_pair($1->GetType(), make_pair($1->GetValue(), $1->GetType()=="ID")));}                                          

Type	    : INT      {$$ = $1;}                                                     
		    | BYTE     {$$ = $1;}                                                    
		    | BOOL     {$$ = $1;}                                                   


// deviding Exp for debug comfort:
Exp         : NumericExp    {$$ = $1; last_exp = $1;}
            | SingleExp     {$$ = $1; last_exp = $1;}
            | complexExp    {$$ = $1; last_exp = $1;}
            ;

NumericExp  : LPAREN Exp RPAREN            {$$ = $2;}                                     
             | Exp MULTIPLICATIVE Exp      {$$ = handeleMultiplacativeAndAdditiveORRelop($1,$3, yylineno);
                                            string regCurType;
                                            string NumbericExpNewRegName = midCode.AddetiveAndMultiplicativeEmit($2->GetValue(), $1->GetValue(), $1->GetType(),$1->GetCurrentRegister(), $3->GetValue(), $3->GetType() , $3->GetCurrentRegister(), regCurType, funcRetType);
                                            $$->SetCurrentRegister(NumbericExpNewRegName);
                                            $$->SetCurrentRegType(regCurType);
                                            }                                 
             | Exp ADDITIVE Exp            {$$ = handeleMultiplacativeAndAdditiveORRelop($1,$3, yylineno);
                                            string regCurType;
                                            string NumbericExpNewRegName = midCode.AddetiveAndMultiplicativeEmit($2->GetValue(), $1->GetValue(), $1->GetType(),$1->GetCurrentRegister(), $3->GetValue(), $3->GetType(), $3->GetCurrentRegister(), regCurType, funcRetType);
                                            $$->SetCurrentRegister(NumbericExpNewRegName);
                                            $$->SetCurrentRegType(regCurType);
                                            }                                    

SingleExp   : ID                      	{$$ = $1;}
            | Call                      {$$ = $1;}                                      
            | NUM                       {$$ = $1;}                                          
            | NUM B                     {checkByteSize($1->GetValue()); $$ = new Node($2->GetType(), $1->GetValue(), yylineno);
                                            string byteReg = midCode.creteRegForByte($1->GetValue());
                                            $$->SetCurrentRegister(byteReg);
                                        }                                              
            | STRING                    {$$ = $1;}                                        
            | TRUE                      {$$ = $1;}                                      
            | FALSE                     {$$ = $1;}                                      

complexExp  : NOT Exp                   {$$ = handleBoolean($2, NULL, yylineno);}                                                     
            | Exp AND MMMMMMMMMMMMMM Exp               {$$ = handleBoolean($1, $4, yylineno);
                                                            midCode.PatchAnd($1, $4, $3, $$);
                                                        }                                         
            | Exp OR  MMMMMMMMMMMMMM Exp                {$$ = handleBoolean($1, $4, yylineno);}                             
            | Exp RELATIONAL Exp        {$$ = handeleMultiplacativeAndAdditiveORRelop($1,$3,yylineno,true);
                                            string resultRegName, resultRegType;
                                            int holeLine = midCode.EmitRelational($1->GetValue(), $1->GetType(),$3->GetValue(), $3->GetType(), $2->GetValue(), resultRegName , resultRegType);
                                            $$->SetCurrentRegister(resultRegName);
                                            $$->SetCurrentRegType(resultRegType);
                                         $$->MergeToTrueList(makelist(make_pair(holeLine, FIRST)));
                                         $$->MergeToFalseList(makelist(make_pair(holeLine, SECOND)));}                                        
            | Exp EQUALITY Exp          {$$ = handeleMultiplacativeAndAdditiveORRelop($1,$3,yylineno,true);
                                            //TODO: same as RELATIONAL 
                                        }                                         
            | LPAREN Type RPAREN Exp    { checkIfConversionLegit($2->GetType(), $4->GetType(), $4->GetValue(), yylineno); $$ = new Node ($2->GetType(), $4->GetValue(), yylineno); }                                   

//markers
M_GLOBAL_SCOPE:       {scopStack.PushNewScope("GLOBAL","");}

M_FUNC_SCOPE:         {scopStack.PushNewScope("FUNC", functionReturnType, types_names_arg_vector);}
M_REGULAR_SCOPE:      {scopStack.PushNewScope("REGULAR","");}
M_WHILE_SCOPE:        {scopStack.PushNewScope("WHILE","");}
M_IF_SCOPE:           {scopStack.PushNewScope("IF","");}
M_ELSE_SCOPE:         {scopStack.PushNewScope("ELSE","");}
M_CHECK_BOOL:         {checkIfExpIsBool();}
M_POP_IF_SCOPE:       {popScope();}

//N M markers for BP
MMMMMMMMMMMMMM :                   {$$ = new Node("NONE","NONE",yylineno); $$->SetLabel(midCode.CreateNewLabel());}
NNNNNNNNNNNNNN :                   {$$ = new Node("NONE","NONE",yylineno); $$->MergeToNextList(midCode.CreateNewPlaceToPatch());}
%%


void yyerror(const char*){   // override the defult error
    output::errorSyn(yylineno);
    exit(1);
}

void checkByteSize(string byteValString){
    int byteVal = std::stoi(byteValString);
    if(byteVal > MAX_BYTE) handleErrorCode(ERROR_BYTE_TOO_LARGE, yylineno, "" ,byteValString);
}

void checkRet(string valType, string valName, int yylino) {
    if (valType == "ID") {
        bool found = true;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(valName, &found);
        if (!found){
            handleErrorCode(ERROR_UNDEF, yylineno, valName ,"needToAddValueForByteError");
        }
        valType = element->GetType().GetTypeName();
    }
    if (!scopStack.CurScopeRetTypeEquals(valType)) {
        handleErrorCode(ERROR_MISMATCH, yylineno, "" ,"needToAddValueForByteError");
    }    
}

void checkIfExpIsBool () {
	Node* exp = last_exp;
    string type = exp->GetType();
    if (exp->GetType() == "ID") {
        bool found = true;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(exp->GetValue(), &found);
        if (!found){
            handleErrorCode(ERROR_UNDEF, yylineno, exp->GetValue() ,"needToAddValueForByteError");
        }
        type = element->GetType().GetTypeName();
    }
    if (type != "BOOL") {
        handleErrorCode(ERROR_MISMATCH, yylineno, "NONE" ,"needToAddValueForByteError");
    }
}

void checkIfConversionLegit(string new_type, string old_type, string name, int yylineno) {
    if (old_type == "ID") {
        bool found = true;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(name, &found);
        if (!found){
            handleErrorCode(ERROR_UNDEF, yylineno, name ,"needToAddValueForByteError");
        }
        old_type = element->GetType().GetTypeName();
    }
    if ( (new_type!="INT" && new_type!= "BYTE")  || (old_type!="INT" && old_type!= "BYTE")) {
        handleErrorCode (ERROR_MISMATCH, yylineno, name ,"needToAddValueForByteError");
    }
}

string GetFunctionReturnTypeByName (string name) {
    string toReturnType = scopStack.GetFunctionReturnType(name,types_names_isId_arg_vector);
    types_names_isId_arg_vector.clear();
    return toReturnType;   
}


bool areAllArgsNamesUnique(const vector<string>& names, string funcName, string* dupName) {
    std::unordered_set<string> uniqueNames; 
    for (const auto& name : names) {
        if ( name == funcName || uniqueNames.find(name) != uniqueNames.end()) {
            *dupName = name;
            // Duplicate name found
            return false;
        }
        uniqueNames.insert(name);
    } 
    // No duplicates found
    return true;
}

void addFuncToScope(bool isOverride,string returnType,string funcName, int yylineno){
    if (isOverride && funcName == "main") {
        handleErrorCode(ERROR_MAIN_OVERRIDE, yylineno, "NON","needToAddValueForByteError");
    }
    vector<string> arguments_types;
    vector<string> arguments_names;
    for (auto pair : types_names_arg_vector) {
            arguments_types.push_back(pair.first);
            arguments_names.push_back(pair.second);
    }

    string dupName;
    if (!areAllArgsNamesUnique(arguments_names,funcName,&dupName)){
        handleErrorCode(ERROR_DEF, yylineno, dupName,"needToAddValueForByteError");
    }

    TypeStruct funcType(returnType,arguments_types,isOverride);
    SymbolTableElement funcToAdd(funcName,funcType);
    ErrorType error = scopStack.checkForErrorBeforeAddSymbolToCurrentScope(funcToAdd);
    if(error == NO_ERROR){
		scopStack.AddSymbolToCurrentScope(funcToAdd);
	}
    else{
        handleErrorCode(error, yylineno, funcName,"needToAddValueForByteError");
    }

}


void assignNewValToExistName(string name,Node* givenNodeVal, int lineno) {
    ErrorType error;
    string givenType = givenNodeVal->GetType();
    string givenName = givenNodeVal->GetValue(); // for x=y if y type is "ID" we need the name name to find real type 
   
    if(givenType != "ID"){
        error = scopStack.checkIfVarExist(name,givenType);
    }
    else{
        bool found = true;
        SymbolTableElement* element = scopStack.SearchInAllScopesByName(givenName, &found);
        if(!found) {
            error = ERROR_UNDEF;
            name = givenName;   //trying to do x=y but y doesn exsist
         }else{                          // trying to do x=y so need to check y real type and not as "ID"
            error = scopStack.checkIfVarExist(name,element->GetType().GetTypeName());
        }
    }
    
    if(error != NO_ERROR) {
         handleErrorCode(error, lineno, name,"needToAddValueForByteError");
    }
}

void checkAfterCallIfFuncExist (string func_name) {
    string var_error_name;
    ErrorType error = scopStack.checkIfAllArgumentsExist(types_names_isId_arg_vector , &var_error_name );  
    if (error != NO_ERROR) {
        handleErrorCode(error, yylineno, var_error_name,"needToAddValueForByteError");
    }

    error =  scopStack.checkAfterCallIfFuncExist(func_name, types_names_isId_arg_vector);  
    if (error != NO_ERROR) {
        handleErrorCode(error, yylineno, func_name,"needToAddValueForByteError");
    }
}

void addElementToScope(string type, string name, string valueType, string valueName , int yylineno) {
    TypeStruct elementType (type);
    SymbolTableElement elementToAdd(name, elementType);
	ErrorType error = scopStack.checkForErrorBeforeAddSymbolToCurrentScope(elementToAdd);
    ErrorType typeError = NO_ERROR;

    if (valueType == "ID") { typeError = scopStack.checkIfVarExist(valueName,type); } 
    if (valueType != "NO_VALUE" && valueType!= "ID") { typeError = scopStack.checkIfAssignedTypesAreCompatible(type, valueType); }
    if(error == NO_ERROR && typeError == NO_ERROR) {
		    scopStack.AddSymbolToCurrentScope(elementToAdd);
    } else {
        error == NO_ERROR ? handleErrorCode(typeError, yylineno, name,"needToAddValueForByteError"):handleErrorCode(error, yylineno, name,"needToAddValueForByteError") ;   
    }
}

Node* handleBoolean (Node* exp1,Node* exp2 ,int yylineno) {
    ErrorType error;
    string exp1Type = exp1->GetType();
    string exp2Type;
    if (exp2!=NULL) {
        exp2Type = exp2->GetType();
    } else {
        exp2Type = "BOOL";
    }  
    // check if exp is BOOL/ID if non of them so error
    if((exp1Type!="BOOL" && exp1Type!="ID" ) || (exp2Type!="BOOL" && exp2Type!="ID")){
        handleErrorCode(ERROR_MISMATCH, yylineno, "NONE" ,"needToAddValueForByteError");
    }

    // check if exist
    bool found1 = true;
    bool found2 = true;
    SymbolTableElement* element1;
    SymbolTableElement* element2;
    if(exp1Type=="ID"){
        element1 = scopStack.SearchInAllScopesByName(exp1->GetValue(), &found1);
    }
    if (!found1){
        handleErrorCode(ERROR_UNDEF, yylineno, exp1->GetValue() ,"needToAddValueForByteError");
    }
    if (exp2Type=="ID"){
        element2 = scopStack.SearchInAllScopesByName(exp2->GetValue(), &found2);
    }
    if (!found2){
        handleErrorCode(ERROR_UNDEF, yylineno, exp2->GetValue() ,"needToAddValueForByteError");
    }  

    // check if type match
    if (exp1Type=="ID") {
        exp1Type = element1->GetType().GetTypeName();
    }
    if (exp2Type=="ID") {
        exp2Type = element2->GetType().GetTypeName();
    }
    if ((exp1Type!="BOOL")|| (exp2Type!="BOOL")) {
        handleErrorCode(ERROR_MISMATCH, yylineno, "NONE" ,"needToAddValueForByteError");
    }

    //create new node
    Node* newNode = new Node("BOOL", "NONE", yylineno);
    return newNode;
}

Node* handeleMultiplacativeAndAdditiveORRelop(Node* exp1,Node* exp2 ,int yylineno, int isRelop /*= false*/) {
    ErrorType error;
    string exp1Type = exp1->GetType();
    string exp2Type = exp2->GetType();
    // check if exp is INT/BYTR/ID if non of them so error
        if((exp1Type!="INT" && exp1Type!="BYTE" && exp1Type!="ID" ) || (exp2Type!="INT" && exp2Type!="BYTE" && exp2Type!="ID")){
        handleErrorCode(ERROR_MISMATCH, yylineno, "NONE" ,"needToAddValueForByteError");
    }
    // check if exist
    bool found1 = true;
    bool found2 = true;
    SymbolTableElement* element1;
    SymbolTableElement* element2;
    if(exp1Type=="ID"){
        element1 = scopStack.SearchInAllScopesByName(exp1->GetValue(), &found1);
    }
    if(!found1){
        handleErrorCode(ERROR_UNDEF, yylineno, exp1->GetValue() ,"needToAddValueForByteError");
    }
    if(exp2Type=="ID"){
        element2 = scopStack.SearchInAllScopesByName(exp2->GetValue(), &found2);
    }
    if(!found2){
        handleErrorCode(ERROR_UNDEF, yylineno, exp2->GetValue() ,"needToAddValueForByteError");
    }  

    // check if type match
    if(exp1Type=="ID") {
        exp1Type = element1->GetType().GetTypeName();
    }
    if(exp2Type=="ID") {
        exp2Type = element2->GetType().GetTypeName();
    }
    if((exp1Type!="INT" && exp1Type!="BYTE") || (exp2Type!="INT" && exp2Type!="BYTE")){
        handleErrorCode(ERROR_MISMATCH, yylineno, "NONE" ,"needToAddValueForByteError");
    }

    // decide what type tp retuen
    string returnType;
    if (!isRelop){       
        if (exp1Type == "INT" || exp2Type == "INT") {
            returnType = "INT";
        } else {
            returnType = "BYTE";
        }
    } else {
        returnType = "BOOL";
    }
    //create new node
    Node* newNode = new Node(returnType, "NONE", yylineno);
    return newNode;

}

void IsCurrentScopeWhile(string type){
	if(!scopStack.IsCurrentScopeWhile()){
		ErrorType error = type == "BREAK"? ERROR_UNEXPECTED_BREAK : ERROR_UNEXPECTED_CONTINUE;
		handleErrorCode(error, yylineno, "NONE" ,"needToAddValueForByteError");
	}
}



void popScope(){
    scopStack.PopScope();
}

int main(){
    // for debug remove before submiting :
    // yydebug = 1;
    yyparse();
    midCode.SetGlobalAndPrintToBuffer();
    midCode.PrintBuffer();
    return 0;
}

void handleErrorCode(ErrorType error, int lineno, const string& name, const string& value) {
    switch (error) {
        case ERROR_LEX:
            output::errorLex(lineno);
            break;
        case ERROR_SYN:
            output::errorSyn(lineno);
            break;
        case ERROR_UNDEF:
            output::errorUndef(lineno, name);
            break;
        case ERROR_UNDEF_FUNC:
            output::errorUndefFunc(lineno, name);
            break;
        case ERROR_DEF:
            output::errorDef(lineno, name);
            break;
        case ERROR_PROTOTYPE_MISMATCH:
            output::errorPrototypeMismatch(lineno, name);
            break;
        case ERROR_MISMATCH:
            output::errorMismatch(lineno);
            break;
        case ERROR_UNEXPECTED_BREAK:
            output::errorUnexpectedBreak(lineno);
            break;
        case ERROR_UNEXPECTED_CONTINUE:
            output::errorUnexpectedContinue(lineno);
            break;
        case ERROR_MAIN_MISSING:
            output::errorMainMissing();
            break;
        case ERROR_BYTE_TOO_LARGE:
            output::errorByteTooLarge(lineno, value);
            break;
        case ERROR_FUNC_NO_OVERRIDE:
            output::errorFuncNoOverride(lineno, name);
            break;
        case ERROR_OVERRIDE_WITHOUT_DECLARATION:
            output::errorOverrideWithoutDeclaration(lineno, name);
            break;
        case ERROR_AMBIGUOUS_CALL:
            output::errorAmbiguousCall(lineno, name);
            break;
        case ERROR_MAIN_OVERRIDE:
            output::errorMainOverride(lineno);
            break;
        default:
            std::cout << "Unknown error." << std::endl;
            break;
    }
    exit(0);
}


// added for hw5:
vector<pair<int,BranchLabelIndex>> makelist (pair<int,BranchLabelIndex> item) {
    vector<pair<int,BranchLabelIndex>> newList;
	newList.push_back(item);
	return newList;
}