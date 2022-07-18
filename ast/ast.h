#pragma once
#include <string>
#include <memory>
#include "ExprAST.h"
#include <vector>
#include "../tokens.h"

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST
{
  int Val;

public:
  NumberExprAST(double Val) : Val(Val) {}
  virtual void print(std::ostream &os) const override;
  virtual llvm::Value *codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
  std::string Name;

public:
  virtual llvm::Value *codegen() override;
  VariableExprAST(const std::string &Name) : Name(Name) {}
  virtual void print(std::ostream &os) const override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
  TOKENS_TYPE Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  virtual llvm::Value *codegen() override;
  BinaryExprAST(TOKENS_TYPE Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  virtual void print(std::ostream &os) const override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST
{
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  virtual llvm::Value *codegen() override;
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}
  virtual void print(std::ostream &os) const override;
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST : public ExprAST
{
  std::string Name;
  std::vector<std::string> Args;

public:
  virtual llvm::Value *codegen() override;
  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
      : Name(Name), Args(Args) {}

  const std::string &getName() const { return Name; }
  const std::vector<std::string> &getArgs() const { return Args; }
  virtual void print(std::ostream &os) const override;
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public ExprAST
{
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  virtual llvm::Value *codegen() override;
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}
  void print(std::ostream &os) const override;
};

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST
{
  std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  virtual llvm::Value *codegen() override;
  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}
  virtual void print(std::ostream &os) const override;
};

void init();
void printModule(llvm::raw_ostream &os);
// end anonymous namespace