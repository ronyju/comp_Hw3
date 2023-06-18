/******************************Definitions section***********************/
%{/*Declaration section*/
#include "hw3_output.hpp"
#include "types.h"
#include "databases.hpp"
#include "parser.tab.hpp"
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

void                {yylval=new Node("VOID", "NONE" ,yylineno); return VOID;}
int                 {yylval=new  Node("INT", "NONE" ,yylineno); return INT;}
byte                {yylval=new  Node("BYTE", "NONE" ,yylineno); return BYTE;}
b                   {yylval=new  Node("BYTE", "NONE" ,yylineno); return B;}
bool                {yylval=new  Node("BOOL", "NONE" ,yylineno); return BOOL;}
override            {yylval=new  Node(yytext, "NONE" ,yylineno); return OVERRIDE;}
and                 {yylval=new  Node("BOOL", yytext, yylineno); return AND;}
or                  {yylval=new  Node("BOOL", yytext, yylineno); return OR;}
not                 {yylval=new  Node("BOOL", yytext, yylineno); return NOT;}
true                {yylval=new  Node("BOOL", yytext, yylineno); return TRUE;}
false               {yylval=new  Node("BOOL", yytext, yylineno); return FALSE;}
return              {yylval=new Node(yytext, "NONE" ,yylineno); return RETURN;}
if                  {yylval=new Node(yytext, "NONE" ,yylineno); return IF;}
else                {yylval=new Node(yytext, "NONE" ,yylineno); return ELSE;}
while               {yylval=new Node(yytext, "NONE" ,yylineno); return WHILE;}
break               {yylval=new Node("BREAK", "NONE" ,yylineno); return BREAK;}
continue            {yylval=new Node("CONTINUE", "NONE" ,yylineno); return CONTINUE;}
{sc}                {yylval=new Node(yytext, "NONE" ,yylineno); return SC;}
{comma}             {yylval=new Node(yytext, "NONE" ,yylineno); return COMMA;}
{lparen}            {yylval=new Node(yytext, "NONE" ,yylineno); return LPAREN;}
{rparem}            {yylval=new Node(yytext, "NONE" ,yylineno); return RPAREN;}
{lbrace}            {yylval=new Node(yytext, "NONE" ,yylineno); return LBRACE;}
{rbace}             {yylval=new Node(yytext, "NONE" ,yylineno); return RBRACE;}
{assign}            {yylval=new Node(yytext, "NONE" ,yylineno); return ASSIGN;}



{relational}        {yylval=new Node("BOOL" , yytext, yylineno); return RELATIONAL;}
{equality}          {yylval=new Node("BOOL" , yytext ,yylineno); return EQUALITY;}


{additive}          {yylval=new Node("NUM" , yytext, yylineno); return ADDITIVE;}
{multiplicative}    {yylval=new Node("NUM" ,yytext, yylineno); return MULTIPLICATIVE;}




{id}         {yylval=new Node("ID", yytext ,yylineno); return ID;}
{num}        {yylval=new Node("INT", yytext ,yylineno); return NUM;}
{string}     {yylval=new Node("STRING", yytext ,yylineno); return STRING;}
{spaces}     ;
{comment}    ;

{error}	     {LexErrorReport(yylineno);} // errorLex and than stop the program.



%%

void LexErrorReport (const int lineno) { 
    output::errorLex(yylineno); 
    exit(0);
}
