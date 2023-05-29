//
// Created by Oren on 25/05/2023.
//
#include <string>
#include <vector>
#include <list>
#include <iostream>
//#include "hw3_output.hpp"
using namespace std;
#ifndef UNTITLED7_TYPES_H
#define UNTITLED7_TYPES_H

/* ------------------------------- Type ------------------------------- */
class Node {
protected:
    int line_number;
    string value;
    string type;


public:
    // constructor for non-function :
    Node(string type, string value, int line_number) : type(type), value(value), line_number(line_number) {}
    virtual ~Node() {};

    string GetType() { return type; }
    string GetValue() { return value; }
    int GetLineNumber() { return line_number; }
};
#define YYSTYPE Node *

/*class Funcs: TypeStruct {
public:
    Funcs(string type, int lineno): TypeStruct(type, lineno, "Funcs"){};
};
class FuncsDecl: public TypeStruct {
public:
    FuncsDecl(int lineno): TypeStruct("NONE", lineno, "FuncsDecl"){};
};
class Override: public TypeStruct {
public:
    Override(string type, int lineno): TypeStruct(type, lineno, "Override"){};
};
class RetType: public TypeStruct {
public:
    RetType(string type, int lineno): TypeStruct(type, lineno, "RetType"){};
};
class Formals: public TypeStruct {
public:
    Formals(string type, int lineno): TypeStruct(type, lineno, "Formals"){};
};
class FormalsList: public TypeStruct {
public:
    FormalsList(string type, int lineno): TypeStruct(type, lineno, "FormalsList"){};
};
class FormalsDecl:public TypeStruct {
public:
    FormalsDecl(string type, int lineno): TypeStruct(type, lineno, "FormalsDecl"){};
};
class Statement: public TypeStruct {
public:
    Statement(string type, int lineno): TypeStruct(type, lineno, "Statement"){};
};
class OpenIfWhileStatement: public TypeStruct {
public:
    OpenIfWhileStatement(string type, int lineno): TypeStruct(type, lineno, "OpenIfWhileStatement"){};
};
class ClosedIfWhileStatment: public TypeStruct {
public:
    ClosedIfWhileStatment(string type, int lineno): TypeStruct(type, lineno, "ClosedIfWhileStatment"){};
};
class Type: public TypeStruct {
public:
    Type(string type, int lineno): TypeStruct(type, lineno, "Type"){};
};

class Exp: public TypeStruct {
public:
    Exp(string type, int lineno): TypeStruct(type, lineno, "Exp"){};
};
class NumericExp:  public TypeStruct {
public:
    NumericExp(TypeStruct* t1, TypeStruct* t2, string op, int lineno): TypeStruct(type, lineno, "NumericExp"){};
};
class SingleExp: public TypeStruct {
public:
    SingleExp(string type,string expType, int lineno): TypeStruct(type, lineno, "SingleExp"){};
    SingleExp(TypeStruct* t1, string expType, int lineno): TypeStruct(t1->GetTypeName(), lineno, "SingleExp"){};
};
class NotIfWhileStatement: public TypeStruct {
public:
    NotIfWhileStatement(Type* type,SingleExp* id,Exp* exp, int lineno): TypeStruct(type->GetTypeName(), lineno, "NotIfWhileStatement"){
    };
};

class Call: public TypeStruct {
public:
    Call(string type, int lineno): TypeStruct(type, lineno, "Call"){};
};
class ExpList: public  TypeStruct {
public:
    ExpList(string type, int lineno): TypeStruct(type, lineno, "ExpList"){};
};



class complexExp:public TypeStruct {
public:
    complexExp(TypeStruct* t1, string op, int lineno): TypeStruct(type, lineno, "complexExp"){};
    complexExp(TypeStruct* t1, TypeStruct* t2, string op, int lineno): TypeStruct(type, lineno, "complexExp"){};

};*/

#endif //UNTITLED7_TYPES_H
