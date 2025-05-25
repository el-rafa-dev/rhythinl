// r_lex.h
#ifndef R_LEX_H
#define R_LEX_H

#include "t_tokens.h"
#include "lex_types.h"
#include <string>

namespace Rythin
{
    class Lexer
    {
    private:
        std::string code_input;
        char current_input;
        int position;
        int line;
        int column;

        void advance_tk();
        void skip_withspace();
        Tokens identifier();
        Tokens stringLiteral();
        char peekNextChar() const;
        std::string processInterpolation();
        std::string digits();

    public:
        Lexer(std::string input);
        Tokens next_tk();
    };
} // namespace Rythin

#endif // R_LEX_H