#pragma once

#include <memory>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

namespace util
{
    static std::unique_ptr<llvm::LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
    static std::unique_ptr<llvm::Module> TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);
    static std::unique_ptr<llvm::IRBuilder<>> Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    static std::map<std::string, llvm::Value *> NamedValues;
};