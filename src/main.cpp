#include <iostream>
#include <string>
#include "tokens.h"
#include "ast/ast.h"
#include <cassert>
#include <fstream>
#include "parser.h"
#include "util.h"
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    // util::init();
    std::ifstream is(argv[1]);
    // llvm::StructType::create();
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