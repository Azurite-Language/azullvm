#include "../ast/ast.h"
#include <iostream>

void NumberExprAST::print(std::ostream &os) const
{
    os << Val;
}

void VariableExprAST::print(std::ostream &os) const
{
    os << Name;
}

void BinaryExprAST::print(std::ostream &os) const
{
    os << "(" << getTokenName(Op) << " ";
    LHS->print(os);
    os << " ";
    RHS->print(os);
    os << ")";
}

void CallExprAST::print(std::ostream &os) const
{
    os << Callee << "(";
    for (auto it = Args.begin(); it != Args.end(); ++it)
    {
        if (it != Args.begin())
            os << ", ";
        (*it)->print(os);
    }
    os << ")";
}

void PrototypeAST::print(std::ostream &os) const
{
    os << "prototype " << Name << "(";
    for (auto it = Args.begin(); it != Args.end(); ++it)
    {
        if (it != Args.begin())
            os << ", ";
        os << *it;
    }
    os << ")";
}

void FunctionAST::print(std::ostream &os) const
{
    Proto->print(os);
    Body->print(os);
}

void IfExprAST::print(std::ostream &os) const
{
    os << "if ";
    Cond->print(os);
    os << " ";
    Then->print(os);
    if (Else)
    {
        os << " else ";
        Else->print(os);
    }
}

void AllocaExprAST::print(std::ostream &os) const
{
    os << "alloca " << Type << " ";
}

void VariadicOperatorExprAST::print(std::ostream &os) const
{
    os << "variadic ";
    for (auto &it : Args)
    {
        it->print(os);
    }
}