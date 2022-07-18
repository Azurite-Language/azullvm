#pragma once
#include "ast/ast.h"

#include <memory>

inline TOKENS getNextToken(std::istream &in)
{
    std::string data;
    in >> data;
    return getToken(data);
}

/**
 * @brief parse the stream of tokens.
 *
 * @param in stream of tokens
 * @return std::unique_ptr<ExprAST> parsed ast
 */
std::unique_ptr<ExprAST> parseToken(std::istream &in);