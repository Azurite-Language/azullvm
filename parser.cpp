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
    assert(getNextToken(is).type == TOKENS_TYPE::RETURN);
    auto return_type = getNextToken(is).value;
    auto cond = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::THEN);
    auto then = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::ELSE);
    auto else_ = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::END_IF);
    return std::make_unique<IfExprAST>(std::move(cond), std::move(then), std::move(else_), return_type);
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
    std::vector<std::string> types;
    TOKENS token = getNextToken(is);
    while (token.type != TOKENS_TYPE::END_USING && is)
    {
        args.push_back(token.value);
        std::string type;
        is >> type;
        types.push_back(type);
        token = getNextToken(is);
    }
    assert(getNextToken(is).type == TOKENS_TYPE::RETURN);
    std::string return_type;
    is >> return_type;
    auto proto = std::make_unique<PrototypeAST>(name, args, types, return_type);
    auto body = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::END_FUNCTION);
    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

std::unique_ptr<AllocaExprAST> parseAllocate(std::istream &is)
{
    assert(getNextToken(is).type == TOKENS_TYPE::TYPE);
    auto type = getNextToken(is).value;
    assert(getNextToken(is).type == TOKENS_TYPE::END_TYPE);

    assert(getNextToken(is).type == TOKENS_TYPE::COUNT);
    auto count = parseToken(is);
    assert(getNextToken(is).type == TOKENS_TYPE::END_COUNT);
    return std::make_unique<AllocaExprAST>(type, std::move(count));
}

std::unique_ptr<VariadicOperatorExprAST> parseVariadic(TOKENS_TYPE type, std::istream &is)
{
    assert(getNextToken(is).type == TOKENS_TYPE::WITH);
    auto token = getNextToken(is);
    std::vector<std::unique_ptr<ExprAST>> body;
    while (token.type != TOKENS_TYPE::END_WITH && is)
    {
        assert(token.type == TOKENS_TYPE::WITH);
        auto expr = parseToken(is);
        assert(getNextToken(is).type == TOKENS_TYPE::END_WITH);
        body.push_back(std::move(expr));
        token = getNextToken(is);
    }
    return std::make_unique<VariadicOperatorExprAST>(type, std::move(body));

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
    if (token.type == TOKENS_TYPE::ALLOCATE)
    {
        return parseAllocate(in);
    }
    if (token.value == "")
    {
        return nullptr;
    }
    if (token.isVariadic())
    {
        return parseVariadic(token.type,in);
    }

    return std::make_unique<VariableExprAST>(token.value);
}