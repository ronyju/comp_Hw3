//declerations
%{
    #include <iostream>
    #include "hw3_output.hpp"
    #include "databases.hpp"
    #include "types.h"
    extern int yylex();
    extern int yylineno;
    extern char *yytext;
    ScopeStack scopStack;
    void popScope();
    void yyerror(const char*);
    string functionReturnType;
	void addElementToScope(string type, string name, string valueType , int yylineno);
    void addFuncToScope(bool isOverride,string returnType,string funcName,int yylineno);
    vector<pair<string,string>> types_names_arg_vector;
    vector<pair<string,pair<string,bool>>> types_names_isId_arg_vector;
    void handleErrorCode(ErrorType error, int lineno, const string& name, const string& value);
    void assignNewValToExistName(string name,string givenType, int lineno);
    Node* handeleMultiplacativeAndAdditive(Node* exp1,Node* exp2,int yylineno);
    void checkAfterCallIfFuncExist (string func_name);


// RONY for debug remove before submiting :
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
Program     : M_GLOBAL_SCOPE Funcs                                                              {popScope();}  

Funcs	    : /*the_empty_word*/                                                    
		    | FuncDecl Funcs                                                      

FuncDecl    : OverRide RetType ID LPAREN Formals RPAREN {addFuncToScope($1->GetType() == "overrid",$2->GetType(),$3->GetValue(),yylineno);} M_FUNC_SCOPE LBRACE Statements RBRACE {popScope();}

OverRide    : /*the_empty_word*/ {$$ = new Node("NONE","NONE",yylineno);}                                                  
		    | OVERRIDE           {$$ = $1;}  //{if($1->IsOverride()) {$$=new Override(true);} else{$$=new Override(false);}}                                                     

RetType     : Type               {$$ = $1;}                                           
	        | VOID               {$$ = $1;}                                                

Formals     : /*the_empty_word*/                                                  
		    | FormalsList                                                          

FormalsList	: FormalDecl                                                          
			| FormalDecl COMMA FormalsList                                        

FormalDecl	: Type ID             {types_names_arg_vector.push_back(make_pair($1->GetType(),$2->GetValue()));}                                               
                                                

// deviding the statment like done in the wikipedia page (https://en.wikipedia.org/wiki/Dangling_else)
// Oren I also added the while as an IF, cuz the compiler showed a S/R conflict with while and inside/after it IF ELSE

Statements	: Statement                                                            
			| Statements Statement  

Statement	: OpenIfWhileStatement
            | ClosedIfWhileStatment
            

OpenIfWhileStatement    : IF LPAREN Exp RPAREN Statement                                        
                        | IF LPAREN Exp RPAREN ClosedIfWhileStatment ELSE OpenIfWhileStatement  
                        | WHILE LPAREN Exp RPAREN OpenIfWhileStatement                          


ClosedIfWhileStatment   : NotIfWhileStatement
                        | IF LPAREN Exp RPAREN ClosedIfWhileStatment ELSE ClosedIfWhileStatment 
                        | WHILE LPAREN Exp RPAREN ClosedIfWhileStatment                        

NotIfWhileStatement     : LBRACE M_REGULAR_SCOPE Statements RBRACE          {popScope();}                                          
                        | Type ID SC                 	                    {addElementToScope($1->GetType(), $2->GetValue() , "NO_VALUE" ,yylineno);}                       
                        | Type ID ASSIGN Exp SC      		                {addElementToScope($1->GetType(), $2->GetValue() , $4->GetType() ,yylineno);}                                             
                        | ID ASSIGN Exp SC           		                {assignNewValToExistName($1->GetValue(),$3->GetType() , yylineno);}                                         
                        | Call SC                                                              
                        | RETURN SC                                                            
                        | RETURN Exp SC                                                       
                        | BREAK SC                                                             
                        | CONTINUE SC                                                         


Call 	    : ID LPAREN ExpList RPAREN    { checkAfterCallIfFuncExist($1->GetValue()); }                                     
            | ID LPAREN RPAREN            { checkAfterCallIfFuncExist($1->GetValue()); }                                        

ExpList	    : Exp	                     {types_names_isId_arg_vector.push_back(make_pair($1->GetType(), make_pair($1->GetValue(), $1->GetType()=="ID")));}                                     
		    | Exp COMMA ExpList          {types_names_isId_arg_vector.push_back(make_pair($1->GetType(), make_pair($1->GetValue(), $1->GetType()=="ID")));}                                          

Type	    : INT      {$$ = $1;}                                                     
		    | BYTE     {$$ = $1;}                                                    
		    | BOOL     {$$ = $1;}                                                   


// deviding Exp for debug comfort:
Exp         : NumericExp    {$$ = $1;}
            | SingleExp     {$$ = $1;}
            | complexExp    {$$ = $1;}
            ;

NumericExp  : LPAREN Exp RPAREN            {$$ = $2;} //{$$ = dynamic_cast<NumericExp*>($2);}                                        
             | Exp MULTIPLICATIVE Exp      {$$ = handeleMultiplacativeAndAdditive($1,$3, yylineno);}//{$$ = new NumericExp($1,$3,"mult",yylineno);}                                  
             | Exp ADDITIVE Exp            {$$ = handeleMultiplacativeAndAdditive($1,$3, yylineno);}//{$$ = new NumericExp($1,$3,"add",yylineno);}                                     

SingleExp   : ID                      	{$$ = $1;} //{$$ = new SingleExp($1->GetTypeName(),"id",yylineno);}           //for id Type.type== name of ID                                
            | Call                      {$$ = $1;}//{$$ = dynamic_cast<SingleExp*>($1);}                                        
            | NUM                       {$$ = $1;}//{$$ = new SingleExp($1->GetTypeName(),"int",yylineno);}                                           
            | NUM B                     {$$ = new Node($2->GetType(), $1->GetValue(), yylineno);}//{$$ = new SingleExp($1->GetTypeName(),"byte",yylineno);}                                              
            | STRING                    {$$ = $1;}//{$$ = new SingleExp($1->GetTypeName(),"string",yylineno);}                                         
            | TRUE                      {$$ = $1;}//{$$ = new SingleExp($1->GetTypeName(),"bool",yylineno);}                                       
            | FALSE                     {$$ = $1;}//{$$ = new SingleExp($1->GetTypeName(),"bool",yylineno);}                                      

complexExp  : NOT Exp                    //{$$ = new complexExp($2,"not",yylineno);}                                          
            | Exp AND Exp                //{$$ = new complexExp($1,$3,"and",yylineno);}                                       
            | Exp OR Exp                 //{$$ = new complexExp($1,$3,"or",yylineno);}                                    
            | Exp RELATIONAL Exp         //{$$ = new complexExp($1,$3,"relational",yylineno);}                                    
            | Exp EQUALITY Exp           //{$$ = new complexExp($1,$3,"equal",yylineno);}                                      
            | LPAREN Type RPAREN Exp     //{$$ = new complexExp($2,$4,"conversion",yylineno);}                                     

//markers
M_GLOBAL_SCOPE:                   {scopStack.PushNewScope("GLOBAL");}
//M2:                 {scopStack.PushNewScope("WHILE");}
//M3:                 {scopStack.PushNewScope("IF");}
//M4:                 {scopStack.PushNewScope("ELSE");}
M_FUNC_SCOPE:         { scopStack.PushNewScope("FUNC",types_names_arg_vector);
                        types_names_arg_vector.clear();}
M_REGULAR_SCOPE:                   {scopStack.PushNewScope("REGULAR");}
//M7:                 {/*need to add*/}

%%

void yyerror(const char*)
{   // override the defult error
    output::errorSyn(yylineno);
    exit(1);
}



void addFuncToScope(bool isOverride,string returnType,string funcName, int yylineno){
    vector<string> arguments_types;
    for (auto pair : types_names_arg_vector) {
            arguments_types.push_back(pair.first);
    }
    TypeStruct funcType(returnType,arguments_types,isOverride);
    SymbolTableElement funcToAdd(funcName,funcType);
    //TODO:oren need to add check for every arg of func
    ErrorType error = scopStack.checkForErrorBeforeAddSymbolToCurrentScope(funcToAdd);
	if(error == NO_ERROR){
		scopStack.AddSymbolToCurrentScope(funcToAdd);
	}
    else{
        handleErrorCode(error, yylineno, funcName,"needToAddValueForByteError");
    }

}


void assignNewValToExistName(string name,string givenType, int lineno) {
    ErrorType error;
    error = scopStack.checkIfVarExist(name,givenType);
	if(error != NO_ERROR) {
        handleErrorCode(error, lineno, name,"needToAddValueForByteError");
    }
}

void checkAfterCallIfFuncExist (string func_name) {
    ErrorType error =  scopStack.checkAfterCallIfFuncExist(func_name, types_names_isId_arg_vector );  
    if (error != NO_ERROR) {
        handleErrorCode(error, yylineno, func_name,"needToAddValueForByteError");
    }
    types_names_isId_arg_vector.clear();
}

void addElementToScope(string type, string name, string valueType , int yylineno) {
    TypeStruct elementType (type);
    SymbolTableElement elementToAdd(name, elementType);
	ErrorType error = scopStack.checkForErrorBeforeAddSymbolToCurrentScope(elementToAdd);
    ErrorType typeError = NO_ERROR; 
    if (valueType != "NO_VALUE") { typeError = scopStack.checkIfAssignedTypesAreCompatible(type, valueType); }
    if(error == NO_ERROR && typeError == NO_ERROR) {
		    scopStack.AddSymbolToCurrentScope(elementToAdd);
    } else {
        error == NO_ERROR ? handleErrorCode(typeError, yylineno, name,"needToAddValueForByteError"):handleErrorCode(error, yylineno, name,"needToAddValueForByteError") ;   
    }
}

Node* handeleMultiplacativeAndAdditive(Node* exp1,Node* exp2 ,int yylineno){
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
    if (exp1Type == "INT" || exp2Type == "INT") {
        returnType = "INT";
    } else {
        returnType = "BYTE";
    }

    //create new node
    Node* newNode = new Node(returnType, "NONE", yylineno);
    return newNode;

}


void popScope(){
    scopStack.PrintCurrentScope();
    scopStack.PopScope();
}

int main()
{
    // RONY for debug remove before submiting :
    // yydebug = 1;

    return yyparse();
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
        default:
            std::cout << "Unknown error." << std::endl;
            break;
    }
    exit(0);
}