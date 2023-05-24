# comp_Hw3

new changes since we spoke:
*offset for function arguments added.
*added enum for all the errors we have, you may use it in your code too.

this are the function you need to use before adding a new var :
ErrorType checkForErrorBeforeAddSymbolToCurrentScope(SymbolTableElement element); 
return values:
ERROR_DEF; // val already exist 
NO_ERROR

this are the function you need to use before adding a new function (the same one):
ERROR_OVERRIDE_WITHOUT_DECLARATION; // trying to override a function that is override without using override in the declaration
ERROR_FUNC_NO_OVERRIDE; // trying to run-over a function the is not override.
ERROR_DEF; // new func is identical to an existing func.
NO_ERROR

!!!NOTICE - YOU must use this function to check all the aruments of the function you are adding as well.  (why? because I just thouth of it now and dont have time to add it myself, if I will have the time I will add it and remove this commant - sorry)

this is the function you need to use when you want to check if a call was ok:
ErrorType checkAfterCallIfFuncExist(string expectedFuncName, vector<string> expectedArgumentsTypes ,string expectedReturnType = "dont care" );// Oren, if you don't care to check the return type dont add it in the arguments
    
return values:
  RROR_UNDEF_FUNC; // calling a function that was not declared.
  ERROR_UNDEF_FUNC; // using identifier that is not a func as a func.
  ERROR_PROTOTYPE_MISMATCH; // no maching arguments 
  ERROR_MISMATCH; //type don't match
  NO_ERROR;
  
  
  
this is the function you need to use after every time a val is being used to check if it was declared:
   ErrorType checkIfVarExist(string expectedVarName, string expectedVarType = "dont care"); //Oren, if you don't care to check the type don't add it in the arguments
    
  return values:
  ERROR_UNDEF; // using a var that was not declared
  ERROR_UNDEF; // using a var that is not a var (a function);
  ERROR_MISMATCH; //type don't match
  NO_ERROR;
  
class in data bases:

<img width="521" alt="image" src="https://github.com/ronyju/comp_Hw3/assets/80697658/ab705903-9175-47f4-8b7c-e04eea8c8d71">

