#include "tokens.h"
#include <unordered_map>
bool isNumber(const std::string &str)
{
    if (str.empty() || (str[0] != '-' && !isdigit(str[0])))
        return false;
    for (char const &c : str)
    {
        if (std::isdigit(c) == 0)
            return false;
    }
    return true;
}

TOKENS getToken(std::string &str)
{
    if (isNumber(str))
        return TOKENS(str, TOKENS_TYPE::NUMBER);
    const static std::unordered_map<std::string, TOKENS_TYPE> tokens_list =
        {
            {"ADD", TOKENS_TYPE::ADD},
            {"SUB", TOKENS_TYPE::SUB},
            {"LT", TOKENS_TYPE::LT},
            {"WITH", TOKENS_TYPE::WITH},
            {"END_WITH", TOKENS_TYPE::END_WITH},
            {"USING", TOKENS_TYPE::USING},
            {"END_USING", TOKENS_TYPE::END_USING},
            {"FUNCTION", TOKENS_TYPE::FUNCTION},
            {"END_FUNCTION", TOKENS_TYPE::END_FUNCTION},
            {"CALL", TOKENS_TYPE::CALL},
            {"END_CALL", TOKENS_TYPE::END_CALL},
            {"IF", TOKENS_TYPE::IF},
            {"THEN", TOKENS_TYPE::THEN},
            {"ELSE", TOKENS_TYPE::ELSE},
            {"END_IF", TOKENS_TYPE::END_IF},
        };
    auto it = tokens_list.find(str);
    if (it != tokens_list.end())
        return TOKENS(str, it->second);
    return TOKENS(str, TOKENS_TYPE::UNKNOWN);
}

const std::string getTokenName(TOKENS_TYPE token)
{
    switch (token)
    {
    case TOKENS_TYPE::ADD:
        return "ADD";
    case TOKENS_TYPE::SUB:
        return "SUB";
    case TOKENS_TYPE::LT:
        return "LT";
    case TOKENS_TYPE::WITH:
        return "WITH";
    case TOKENS_TYPE::END_WITH:
        return "END_WITH";
    case TOKENS_TYPE::NUMBER:
        return "NUMBER";
    case TOKENS_TYPE::UNKNOWN:
        return "UNKNOWN";
    }
    return "";
}