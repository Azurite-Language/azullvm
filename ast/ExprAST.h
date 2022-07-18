#pragma once
#include <istream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

enum class ExprASTType
{
    BinaryExprAST,
    NumberExprAST,
    ExprAST
};

class AST
{
public:
    virtual void print(std::ostream &os) const
    {
        os << "AST";
    }
    virtual ~AST(){};
};

class ExprAST : public AST
{
public:
    virtual ~ExprAST(){};

    virtual void print(std::ostream &os) const override
    {
        os << "ExprAST";
    }
    virtual llvm::Value *codegen() = 0;
};