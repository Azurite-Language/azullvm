#include "parser.h"

#include <istream>
#include <string>
#include "tokens.h"

#include <cassert>

/**
 * @brief create if ast from tokens.
 *
 * @param is Next token should be COND ("check will be performed")
 * @return std::unique_ptr<IfExprAST> IfExprAST
 */
std::unique_ptr<IfExprAST> parseIf(std::istream &is)
{
    auto cond = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::THEN);
    auto then = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::ELSE);
    auto else_ = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::END_IF);
    return std::make_unique<IfExprAST>(std::move(cond), std::move(then), std::move(else_));
}

/**
 * @brief create function ast from tokens.
 *
 * @param is Next token should be function name ("check will be performed")
 * @return std::unique_ptr<FunctionAST> FunctionAST
 */
std::unique_ptr<FunctionAST> parseFunction(std::istream &is)
{
    std::string name;
    is >> name;
    assert(getNextToken(is).type == TOKENS_TYPE::USING);
    std::vector<std::string> args;
    TOKENS token = getNextToken(is);
    while (token.type != TOKENS_TYPE::END_USING && is)
    {
        args.push_back(token.value);
        token = getNextToken(is);
    }
    auto proto = std::make_unique<PrototypeAST>(name, args);
    auto body = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::END_FUNCTION);
    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

std::unique_ptr<ExprAST> parseToken(std::istream &in)
{
    TOKENS token = getNextToken(in);
    if (token.isOperator())
    {
        assert(getNextToken(in).type == TOKENS_TYPE::WITH);
        auto lhs = parseToken(in);
        assert(getNextToken(in).type == TOKENS_TYPE::END_WITH);
        assert(getNextToken(in).type == TOKENS_TYPE::WITH);
        auto rhs = parseToken(in);
        assert(getNextToken(in).type == TOKENS_TYPE::END_WITH);
        return std::make_unique<BinaryExprAST>(token.type, std::move(lhs), std::move(rhs));
    }
    if (token.type == TOKENS_TYPE::NUMBER)
    {
        int val = std::stoi(token.value);
        return std::make_unique<NumberExprAST>(NumberExprAST(val));
    }
    if (token.type == TOKENS_TYPE::FUNCTION)
    {
        return parseFunction(in);
    }
    if (token.type == TOKENS_TYPE::CALL)
    {
        std::string callee;
        in >> callee;
        std::vector<std::unique_ptr<ExprAST>> args;
        auto token = getNextToken(in);
        while (token.type == TOKENS_TYPE::WITH && in)
        {
            args.push_back(parseToken(in));
            assert(getNextToken(in).type == TOKENS_TYPE::END_WITH);
            token = getNextToken(in);
        }
        assert(token.type == TOKENS_TYPE::END_CALL);
        return std::make_unique<CallExprAST>(callee, std::move(args));
    }
    if (token.type == TOKENS_TYPE::IF)
    {
        return parseIf(in);
    }
    if (token.value == "")
    {
        return nullptr;
    }

    return std::make_unique<VariableExprAST>(token.value);
}