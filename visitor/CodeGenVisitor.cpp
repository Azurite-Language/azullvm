#include "../ast/ast.h"
#include "../util.h"

using namespace llvm;

void printModule(llvm::raw_ostream &os)
{
    util::TheModule->print(os, nullptr);
}

llvm::Value *NumberExprAST::codegen()
{
    return llvm::ConstantInt::get(get_type("num"), Val);
}

llvm::Value *LogErrorV(const char *Str)
{
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

llvm::Value *VariableExprAST::codegen()
{
    // Look this variable up in the function.
    llvm::Value *V = util::NamedValues[Name];
    if (!V)
        LogErrorV("Unknown variable name");
    return V;
}

llvm::Value *BinaryExprAST::codegen()
{
    llvm::Value *L = LHS->codegen();
    llvm::Value *R = RHS->codegen();
    if (!L || !R)
        return nullptr;

    llvm::Value *Res = nullptr;
    switch (Op)
    {
    case TOKENS_TYPE::ADD:
        Res = util::Builder->CreateAdd(L, R, "addtmp");
        break;
    case TOKENS_TYPE::SUB:
        Res = util::Builder->CreateSub(L, R, "subtmp");
        break;
    case TOKENS_TYPE::LT:
        Res = util::Builder->CreateICmpULT(L, R, "cmptmp");
        break;
    case TOKENS_TYPE::MUL:
        Res = util::Builder->CreateMul(L, R, "multmp");
        break;
    case TOKENS_TYPE::NTH:
    {
        auto t = (llvm::AllocaInst *)L;
        Res = util::Builder->CreateLoad(util::Builder->CreateGEP(t, R, "nthtmp"));
    }
    break;
    // case TOKENS_TYPE::DIVIDE:
    //     Res = Builder->CreateFDiv(L, R, "divtmp");
    //     break;
    default:
        LogErrorV("invalid binary operator");
        break;
    }
    return Res;
}

llvm::Value *CallExprAST::codegen()
{
    // Look up the name in the global module table.
    llvm::Function *CalleeF = util::TheModule->getFunction(Callee);
    if (!CalleeF)
        return LogErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != Args.size())
        return LogErrorV("Incorrect # of arguments passed");

    std::vector<llvm::Value *> ArgsV;
    for (unsigned i = 0, e = Args.size(); i != e; ++i)
    {
        llvm::Value *ArgValue = Args[i]->codegen();
        if (!ArgValue)
            return nullptr;
        ArgsV.push_back(ArgValue);
    }
    return util::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Value *PrototypeAST::codegen()
{

    std::vector<llvm::Type *> types;
    for (auto &arg : Types)
    {
        types.push_back(get_type(arg));
    }
    llvm::FunctionType *FT = llvm::FunctionType::get(get_type(ReturnType), types, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, util::TheModule.get());

    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(Args[Idx++]);
    return F;
}

llvm::Value *FunctionAST::codegen()
{
    llvm::Function *TheFunction = util::TheModule->getFunction(Proto->getName());
    if (!TheFunction)
        TheFunction = (llvm::Function *)Proto->codegen();

    if (!TheFunction)
        return nullptr;

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*util::TheContext, "entry", TheFunction);
    util::Builder->SetInsertPoint(BB);
    util::NamedValues.clear();
    for (auto &Arg : TheFunction->args())
        util::NamedValues[Arg.getName()] = &Arg;
    if (llvm::Value *RetVal = Body->codegen())
    {
        util::Builder->CreateRet(RetVal);
        verifyFunction(*TheFunction);
        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return nullptr;
}

llvm::Value *IfExprAST::codegen()
{
    Value *CondV = Cond->codegen();
    if (!CondV)
        return nullptr;

    Function *TheFunction = util::Builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    auto ThenBB = llvm::BasicBlock::Create(*util::TheContext, "then", TheFunction);
    auto ElseBB = llvm::BasicBlock::Create(*util::TheContext, "else");
    auto MergeBB = llvm::BasicBlock::Create(*util::TheContext, "ifcont");

    util::Builder->CreateCondBr(CondV, ThenBB, ElseBB);

    // Emit then value.
    util::Builder->SetInsertPoint(ThenBB);

    Value *ThenV = Then->codegen();
    if (!ThenV)
        return nullptr;

    util::Builder->CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    ThenBB = util::Builder->GetInsertBlock();

    // Emit else block.
    TheFunction->getBasicBlockList().push_back(ElseBB);
    util::Builder->SetInsertPoint(ElseBB);

    Value *ElseV = Else->codegen();
    if (!ElseV)
        return nullptr;

    util::Builder->CreateBr(MergeBB);
    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = util::Builder->GetInsertBlock();

    // Emit merge block.
    TheFunction->getBasicBlockList().push_back(MergeBB);
    util::Builder->SetInsertPoint(MergeBB);
    PHINode *PN = util::Builder->CreatePHI(get_type(ReturnType), 2, "iftmp");

    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
}

llvm::Value *AllocaExprAST::codegen()
{
    llvm::Value *Size = SizeAST->codegen();
    if (!Size)
        return nullptr;
    return util::Builder->CreateAlloca(get_type("num"), Size, "allocatemp");
}

llvm::Value *VariadicOperatorExprAST::codegen()
{
    llvm::Value *Res = nullptr;
    switch (Op)
    {
    case TOKENS_TYPE::DO:
        for (auto &expr : Args)
        {
            Res = expr->codegen();
        }
        break;
    case TOKENS_TYPE::SET:
    {
        auto t = (llvm::AllocaInst *)Args[0]->codegen();
        Res = util::Builder->CreateStore(Args[2]->codegen(), util::Builder->CreateGEP(t, Args[1]->codegen(), "settmp"));
    }
    break;
    }
    return Res;
}