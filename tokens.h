#pragma once
#include <string>

enum class TOKENS_TYPE
{
    ADD,
    SUB,
    LT,
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
    NUMBER
};

class TOKENS
{
public:
    std::string value;
    TOKENS_TYPE type;
    TOKENS(std::string value, TOKENS_TYPE type) : value(value), type(type) {}
    bool isOperator() const { return type == TOKENS_TYPE::ADD || type == TOKENS_TYPE::SUB || type == TOKENS_TYPE::LT; }
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