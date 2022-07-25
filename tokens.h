#pragma once
#include <string>
#include "llvm/IR/DerivedTypes.h"
enum class TOKENS_TYPE
{
    ADD,
    SUB,
    LT,
    MUL,
    WITH,
    END_WITH,
    UNKNOWN,
    USING,
    END_USING,
    FUNCTION,
    END_FUNCTION,
    CALL,
    END_CALL,
    IF,
    THEN,
    ELSE,
    END_IF,
    TYPE,
    END_TYPE,
    COUNT,
    END_COUNT,
    ALLOCATE,
    NTH,
    SET,
    DO,
    END_DO,
    NUMBER
};

class TOKENS
{
public:
    std::string value;
    TOKENS_TYPE type;
    TOKENS(std::string value, TOKENS_TYPE type) : value(value), type(type) {}
    bool isOperator() const { return type == TOKENS_TYPE::ADD || type == TOKENS_TYPE::SUB || type == TOKENS_TYPE::LT || type == TOKENS_TYPE::MUL || type == TOKENS_TYPE::NTH; }
    bool isVariadic() const { return type == TOKENS_TYPE::DO || type == TOKENS_TYPE::SET; }
};

enum class OPERATOR
{
    ADD,
};

/**
 * @brief Get the Token from a string.
 *
 * @param str
 * @return TOKENS corresponding token.
 */
TOKENS getToken(std::string &str);

const std::string getTokenName(TOKENS_TYPE token);
llvm::Type *get_type(const std::string type);