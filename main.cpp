#include <iostream>
#include <string>
#include "tokens.h"
#include "ast/ast.h"
#include <cassert>
#include <fstream>
#include "parser.h"
#include "util.h"
int main(void)
{
    // util::init();
    std::ifstream is("output.prellvm");
    std::ofstream os("output.ll");
    while (is.good() || !is.eof() || is)
    {
        auto ast = parseToken(is);
        if (!ast)
            break;
        ast->print(std::cout);
        auto *result = ast->codegen();
        result->print(llvm::outs());
        std::cout << std::endl;
    }

    printModule(llvm::errs());
    return 0;
}