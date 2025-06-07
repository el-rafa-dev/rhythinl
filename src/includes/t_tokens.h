#include "string"
#include "lex_types.h"

#ifndef T_TOKENS_H
#define T_TOKENS_H

struct Tokens
{
    public:
        TokensTypes type;
        std::string value;
        int line;
        int column;

    Tokens(TokensTypes type, std::string value, int line, int column);
    static std::string tokenTypeToString(TokensTypes type);
};

#endif