
//
// Created by Oren on 25/05/2023.
//
#include "types.h"

/*
FuncDecl::FuncDecl(int line_number) : Node("FuncDecl", line_number)
{
    // do nothing
}

RetType::RetType(Node *node) : Node("RetType", node->line_num)
{
    global_ret_type = node->value;
}

Formal::Formal(int line_number) : Node("Formal", line_number) // no params
{
    // checks if ID already exists
    Table global_table = tables_stack[0];
    for (TableEntry &entry : global_table.table_vec)
    {
        if (entry.name.compare(getCurrentFunctionName()) == 0)
        {
            output::errorDef(line_number, getCurrentFunctionName());
            exit(0);
        }
    }

    // add func to global scope
    vector<string> arg_types;
    TableType func_type(true, getCurrentFunctionRet(), arg_types);
    TableEntry func_entry(getCurrentFunctionName(), func_type, 0);
    tables_stack[0].table_vec.push_back(func_entry);
}

Formal::Formal(FuncArgs *func_args) : Node("Formal", func_args->line_num)
{
    // checks if ID already exists
    Table global_table = tables_stack[0];
    for (TableEntry &entry : global_table.table_vec)
    {
        if (entry.name.compare(getCurrentFunctionName()) == 0)
        {
            output::errorDef(func_args->line_num, getCurrentFunctionName());
            exit(0);
        }
    }

    // add func to global scope
    vector<string> arg_types;
    for (auto arg = func_args->args_list.begin(); arg != func_args->args_list.end(); arg++)
    {
        arg_types.push_back((*arg)->arg_type);
    }

    TableType func_type(true, getCurrentFunctionRet(), arg_types);
    TableEntry func_entry(getCurrentFunctionName(), func_type, 0);
    tables_stack[0].table_vec.push_back(func_entry);

    // fill function scope
    int arg_offset = -1;
    for (auto arg = func_args->args_list.begin(); arg != func_args->args_list.end(); arg++)
    {
        TableEntry arg_entry((*arg)->arg_name, TableType(false, (*arg)->arg_type), arg_offset);
        tables_stack.back().table_vec.push_back(arg_entry);
        arg_offset--;
    }

    //print_Stack();
}

Arg::Arg(Node *type, Node *id) : Node("Arg", id->line_num) //(11)
{
    // check if arg's id is taken by current function which is not yet added to global scope symbol table
    if (global_func_name.compare(id->value) == 0)
    {
        output::errorDef(id->line_num, id->value);
        exit(0);
    }

    // check if arg's id is taken by other functions
    Table global_table = tables_stack[0];
    for (TableEntry &entry : global_table.table_vec)
    {
        if (entry.name.compare(id->value) == 0)
        {
            output::errorDef(id->line_num, id->value);
            exit(0);
        }
    }
    arg_type = type->value;
    arg_name = id->value;
}

FuncArgs::FuncArgs(int line_number) : Node("FuncArgs", line_number)
{
    // do nothing
}

FuncArgs::FuncArgs(Arg *arg) : Node("FuncArgs", arg->line_num) //(9)
{
    this->args_list = list<Arg *>{arg};
}

FuncArgs::FuncArgs(Arg *arg, FuncArgs *func_args) : Node("FuncArgs", arg->line_num) //(10)
{
    this->args_list = func_args->args_list;
    // check if arg's name (id) is not taken by other arg's name
    for (Arg* current_arg : this->args_list)
    {
        if (current_arg->arg_name.compare(arg->arg_name) == 0)
        {
            output::errorDef(func_args->line_num, arg->arg_name);
            exit(0);
        }
    }
    this->args_list.push_front(arg);
}

Statement::Statement(Node *node) : Node("Statement", node->line_num)
{
    // do nothing
}

OpenStatement::OpenStatement(Expression *exp) : Node("OpenStatement", exp->line_num) // 21, 22, 23
{
    */
/*if (exp->type.compare("bool") != 0)
    {
        output::errorMismatch(exp->line_num);
        exit(0);
    }*//*

}

ClosedStatement::ClosedStatement(int line_num) : Node("ClosedStatement", line_num) // Closed -> Simple
{
    // do nothing
}

ClosedStatement::ClosedStatement(Expression *exp) : Node("ClosedStatement", exp->line_num) // 22, 23
{
    */
/*if (exp->type.compare("bool") != 0)
    {
        output::errorMismatch(exp->line_num);
        exit(0);
    }*//*

}

SimpleStatement::SimpleStatement(Node *type, Node *id) : Node("SimpleStatement", id->line_num) // 15
{
    if (isNameTaken(id->value) == true)
    {
        output::errorDef(id->line_num, id->value);
        exit(0);
    }

    else
    {
        addVarToStacks(id->value, type->value);
    }
}

SimpleStatement::SimpleStatement(Node *type, Node *id, Expression *exp) : Node("SimpleStatement", id->line_num) // 16
{
    // Type ID ASSIGN Exp SC
    // vector<TableEntry> current_scope = tables_stack.back();
    if (isNameTaken(id->value) == true)
    {
        output::errorDef(id->line_num, id->value);
        exit(0);
    }
    string left_type = type->value, right_type = exp->type;
    if ((left_type.compare(right_type) != 0) &&
        !(left_type.compare("int") == 0 && right_type.compare("byte") == 0)) // int x = byte y
    {
        output::errorMismatch(id->line_num);
        exit(0);
    }
    else
    {
        addVarToStacks(id->value, type->value);
    }
}

SimpleStatement::SimpleStatement(string str, Expression *exp) : Node("SimpleStatement", exp->line_num) // 17
{
    if (str.compare("return") == 0) // 20
    {
        string ret_val = getCurrentFunctionRet();
        if (ret_val.compare("void") == 0 || (!(ret_val.compare("int") == 0 && exp->type.compare("byte") == 0) && // allow return byte when func return val is int
                                             ret_val.compare(exp->type) != 0))
        {
            output::errorMismatch(exp->line_num);
            exit(0);
        }
    }
    else // 17
    {
        TableType type;
        if (!isVarIdExist(str, &type))
        {
            output::errorUndef(exp->line_num, str);
            exit(0);
        }

        string left_type = type.var_type, right_type = exp->type;
        if ((left_type.compare(right_type) != 0) &&
            !(left_type.compare("int") == 0 && right_type.compare("byte") == 0))
        {
            output::errorMismatch(exp->line_num);
            exit(0);
        }
    }
}

SimpleStatement::SimpleStatement(Node *node) : Node("SimpleStatement", node->line_num) // 14, 18, 19, 24, 25
{
    if (node->value.compare("call") == 0 || node->value.compare("{") == 0) // 14, 18
    {
        // do nothing
    }
    else if (node->value.compare("return") == 0) // 19
    {
        if (getCurrentFunctionRet().compare("void") != 0)
        {
            output::errorMismatch(node->line_num);
            exit(0);
        }
    }
    else if (node->value.compare("break") == 0) // 24
    {
        if (tables_stack.back().is_while == false)
        {
            output::errorUnexpectedBreak(node->line_num);
            exit(0);
        }
    }
    else if (node->value.compare("continue") == 0) // 25
    {
        if (tables_stack.back().is_while == false)
        {
            output::errorUnexpectedContinue(node->line_num);
            exit(0);
        }
    }
}

ExpList::ExpList(Expression *exp) : Node(exp->value, exp->line_num)
{
    list.push_back(exp);
}

ExpList::ExpList(Expression *exp, ExpList *explist) : Node(exp->value, exp->line_num)
{
    this->list.push_back(exp);
    this->list.insert(this->list.end(), explist->list.begin(), explist->list.end());
}

Expression::Expression(string type_name, Node *node) : Node(node->value, node->line_num) // 36, 38, 39, 40, 41, 42
{
    type = type_name;
    global_exp_type = type;
    if (type_name.compare("id") == 0)
    {
        TableType id_type;
        if (!isVarIdExist(node->value, &id_type))
        {
            output::errorUndef(node->line_num, node->value);
            exit(0);
        }
        this->type = id_type.var_type;
        global_exp_type = this->type;
    }
    else if (type.compare("byte") == 0)
    {
        if (stoi(node->value) > 255)
        {
            output::errorByteTooLarge(node->line_num, node->value);
            exit(0);
        }
    }
}

Expression::Expression(string op, Node *node, Expression *exp) : Node(exp->value, exp->line_num) // 43 ,47
{
    if (op.compare("not") == 0)
    {
        if (exp->type.compare("bool") != 0)
        {
            output::errorMismatch(node->line_num);
            exit(0);
        }
        this->type = "bool";
        global_exp_type = this->type;
    }
    if (op.compare("cast") == 0 && (exp->type.compare("int") == 0 || exp->type.compare("byte") == 0))
    {
        if (node->value.compare("byte") == 0)
        {
            this->type = "byte";
            global_exp_type = this->type;
        }
        else if (node->value.compare("int") == 0)
        {
            this->type = "int";
            global_exp_type = this->type;
        }
        else
        {
            output::errorMismatch(node->line_num);
            exit(0);
        }
    }
}

Expression::Expression(string type_name, Expression *exp1, Node *node, Expression *exp2, string op) : Node(type_name, node->line_num) //(44-46,35-38binops)
{
    if (op.compare("and") == 0 || op.compare("or") == 0)
    {
        if (!(exp1->type.compare("bool") == 0 && exp2->type.compare("bool") == 0))
        {
            output::errorMismatch(node->line_num);
            exit(0);
        }
        this->type = "bool";
        global_exp_type = this->type;
    }
    else if (op.compare("relop") == 0)
    {
        if (!((exp1->type.compare("int") == 0 || exp1->type.compare("byte") == 0) && (exp2->type.compare("int") == 0 || exp2->type.compare("byte") == 0)))
        {
            output::errorMismatch(node->line_num);
            exit(0);
        }
        this->type = "bool";
        global_exp_type = this->type;
    }
    else if (op.compare("binop") == 0)
    {
        this->type = "int"; // default is int
        if ((exp1->type.compare("byte") == 0) && (exp2->type.compare("byte") == 0))
        {
            this->type = "byte";
            global_exp_type = this->type;
        }
        else if (!(((exp1->type.compare("int") == 0) && (exp2->type.compare("int") == 0)) || ((exp1->type.compare("int") == 0) && (exp2->type.compare("byte") == 0)) || ((exp1->type.compare("byte") == 0) && (exp2->type.compare("int") == 0))))
        {
            output::errorMismatch(node->line_num);
            exit(0);
        }
    }
}

Expression::Expression(string op, Expression *exp1, Expression *exp2, Expression *exp3) : Node("Trinary", exp1->line_num) //(34)
{
    if (exp2->type.compare("bool") != 0)
    {
        output::errorMismatch(exp2->line_num);
        exit(0);
    }
    string new_type = exp1->type;
    if (exp1->type.compare(exp3->type) == 0)
    {
        this->type = new_type;
        global_exp_type = this->type;
    }
    else if ((exp1->type.compare("int") == 0) && (exp3->type.compare("byte") == 0) || (exp1->type.compare("byte") == 0) && (exp3->type.compare("int") == 0))
    {
        this->type = "int";
        global_exp_type = this->type;
    }
    else
    {
        output::errorMismatch(exp2->line_num);
        exit(0);
    }
}

Expression::Expression(Node *id, ExpList *explist) : Node("Call", id->line_num) // 26
{
    bool found;
    TableEntry func_entry = getFuncEntry(id, &found);
    if (found == false)
    {
        output::errorUndefFunc(id->line_num, id->value);
        exit(0);
    }

    this->type = func_entry.type.ret_type;
    global_exp_type = this->type;
    TableType &func_type = func_entry.type;
    vector<string> params = func_entry.type.arg_types;
    vector<string> arg_caps;
    for (string str : params)
        arg_caps.push_back(token_string[str]);

    if (params.size() != explist->list.size())
    {
        output::errorPrototypeMismatch(id->line_num, id->value, arg_caps);
        exit(0);
    }

    for (int i = 0; i < params.size(); i++)
    {
        if (explist->list[i]->type.compare((params)[i]) != 0 && !(explist->list[i]->type.compare("byte") == 0 && params[i].compare("int") == 0))
        {
            output::errorPrototypeMismatch(id->line_num, id->value, arg_caps);
            exit(0);
        }
    }
}

Expression::Expression(Node *id) : Node("Call", id->line_num) //(27)
{
    bool found;
    TableEntry func_entry = getFuncEntry(id, &found);
    if (found == false)
    {
        output::errorUndefFunc(id->line_num, id->value);
        exit(0);
    }
    type = func_entry.type.ret_type;
    global_exp_type = type;
    //new
    vector<string> params = func_entry.type.arg_types;
    vector<string> arg_caps;
    for (string str : params)
        arg_caps.push_back(token_string[str]);

    if (params.size() != 0)
    {
        output::errorPrototypeMismatch(id->line_num, id->value, arg_caps);
        exit(0);
    }

}*/
