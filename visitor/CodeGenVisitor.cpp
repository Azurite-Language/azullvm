#include "../ast/ast.h"

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::map<std::string, llvm::Value *> NamedValues;

using namespace llvm;

void printModule(llvm::raw_ostream &os)
{
    TheModule->print(os, nullptr);
}

void init()
{
    // Open a new context and module.
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

llvm::Value *NumberExprAST::codegen()
{
    return llvm::ConstantInt::get(*TheContext, llvm::APInt(32, Val));
}

llvm::Value *LogErrorV(const char *Str)
{
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

llvm::Value *VariableExprAST::codegen()
{
    // Look this variable up in the function.
    llvm::Value *V = NamedValues[Name];
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
        Res = Builder->CreateAdd(L, R, "addtmp");
        break;
    case TOKENS_TYPE::SUB:
        Res = Builder->CreateSub(L, R, "subtmp");
        break;
    case TOKENS_TYPE::LT:
        Res = Builder->CreateICmpULT(L, R, "cmptmp");
        break;
    // case TOKENS_TYPE::MULTIPLY:
    //     Res = Builder->CreateFMul(L, R, "multmp");
    //     break;
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
    llvm::Function *CalleeF = TheModule->getFunction(Callee);
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
    return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Value *PrototypeAST::codegen()
{
    std::vector<llvm::Type *> DoubleArgs(Args.size(), llvm::Type::getInt32Ty(*TheContext));
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext), DoubleArgs, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule.get());

    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(Args[Idx++]);
    return F;
}

llvm::Value *FunctionAST::codegen()
{
    llvm::Function *TheFunction = TheModule->getFunction(Proto->getName());
    if (!TheFunction)
        TheFunction = (llvm::Function *)Proto->codegen();

    if (!TheFunction)
        return nullptr;

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);
    NamedValues.clear();
    for (auto &Arg : TheFunction->args())
        NamedValues[Arg.getName()] = &Arg;
    if (llvm::Value *RetVal = Body->codegen())
    {
        Builder->CreateRet(RetVal);
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

    Function *TheFunction = Builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    auto ThenBB = llvm::BasicBlock::Create(*TheContext, "then", TheFunction);
    auto ElseBB = llvm::BasicBlock::Create(*TheContext, "else");
    auto MergeBB = llvm::BasicBlock::Create(*TheContext, "ifcont");

    Builder->CreateCondBr(CondV, ThenBB, ElseBB);

    // Emit then value.
    Builder->SetInsertPoint(ThenBB);

    Value *ThenV = Then->codegen();
    if (!ThenV)
        return nullptr;

    Builder->CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    ThenBB = Builder->GetInsertBlock();

    // Emit else block.
    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder->SetInsertPoint(ElseBB);

    Value *ElseV = Else->codegen();
    if (!ElseV)
        return nullptr;

    Builder->CreateBr(MergeBB);
    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = Builder->GetInsertBlock();

    // Emit merge block.
    TheFunction->getBasicBlockList().push_back(MergeBB);
    Builder->SetInsertPoint(MergeBB);
    PHINode *PN = Builder->CreatePHI(Type::getInt32Ty(*TheContext), 2, "iftmp");

    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
}