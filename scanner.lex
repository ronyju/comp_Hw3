/******************************Definitions section***********************/
%{/*Declaration section*/
#include "parser.tab.hpp"
#include "output.hpp"
#include <string>
#include <iostream>

#include <stdio.h>
void printToken(char *);
void LexErrorReport(const int);
%}

%option yylineno
%option noyywrap

letter          ([a-zA-Z])
digit           ([0-9])
positive        ([1-9])
zero            (0)
error           (.)


sc              (;)
comma           (,)  
lparen          (\()   
rparem          (\)) 
lbrace          (\{)
rbace           (\})  
assign          (=)

/*relop was divided to relational and equality*/
relop           (==|!=|<|>|<=|>=)
relational      (<|>|<=|>=)
equality        (==|!=)

/*binop was divided to additive and multiplicative:*/
binop           ([+|\-|*|/])
additive        ([+|\-])
multiplicative  ([*|/])

id              [A-Za-z][0-9A-Za-z]*
num             (0|[1-9][0-9]*)
string          (\"([^\n\r\"\\]|\\[rnt"\\])+\")
spaces          ([ \n\r\t])
comment         ("//"[^\r\n]*[\r|\n|\r\n]?)


ascii_syntax    (\\x{ascii_code})
special_string  (\\\"|\\n|\\r|\\\\|\\0|\\t|\\x{ascii_code})
allowed_string  ([^\"\\\n\r])
quotation_mark  (\")
comment_sign    (\/\/)
white_space     (\r|\n|\t|\x20|\x09)
all_but_spaces  ([^\r\n])
illegal_LF      (\n)
illegal_escape  (\\[^\"\\\n\r\t\0])


printable_chars         ([\x20-\x7E|\x09|\x0A|\x0D])
escape_sequence         ([\a|\b|\e|\f|\n|\r|\t|\v|\\|\'|\"|\?])
/*( \x07 | \x08 | \x1B | \x0C | \x0A | \x0D | \x09 | \x0B | \x5C | \x27 | \x22 | \x3F )*/
string_escape_sequence ([\\\\|\\\"|\\n|\\r|\\t|\\0])

/*roles : first simple and then complex*/
/*Oren - the stuff added (-10) as an unti example for num, so only 0 or positive numbers are allowed*/
/* from the lex website - the ? means 0 or 1 occurrences of the preceding expression (which is equivalent to saying that the preceding expression is optional), and the * means 0 or more*/
%%

void                {return VOID;}
int                 {return INT;}
byte                {return BYTE;}
b                   {return B;}
bool                {return BOOL;}
override            {return OVERRIDE;}
and                 {return AND;}
or                  {return OR;}
not                 {return NOT;}
true                {return TRUE;}
false               {return FALSE;}
return              {return RETURN;}
if                  {return IF;}
else                {return ELSE;}
while               {return WHILE;}
break               {return BREAK;}
continue            {return CONTINUE;}
{sc}                {return SC;}
{comma}             {return COMMA;}
{lparen}            {return LPAREN;}
{rparem}            {return RPAREN;}
{lbrace}            {return LBRACE;}
{rbace}             {return RBRACE;}
{assign}            {return ASSIGN;}



{relational}        {return RELATIONAL;}     
{equality}          {return EQUALITY;}  


{additive}          {return ADDITIVE;} 
{multiplicative}    {return MULTIPLICATIVE;} 

  


{id}         {return ID;}
{num}        {return NUM;}
{string}     {return STRING;}
{spaces}     ;   
{comment}    ;

{error}	     {LexErrorReport(yylineno);} // errorLex and than stop the program.



%%

void LexErrorReport (const int lineno) { 
    output::errorLex(yylineno); 
    exit(0);
}