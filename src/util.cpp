#include "util.h"

std::unique_ptr<llvm::LLVMContext> util::TheContext = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module>  util::TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);
std::unique_ptr<llvm::IRBuilder<>> util::Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
std::map<std::string, llvm::Value *> util::NamedValues;
