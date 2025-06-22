// r_lex.h
#ifndef R_LEX_HPP
#define R_LEX_HPP

#include "t_tokens.hpp"
#include "lex_types.hpp"
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

        void advance_tk(bool isComment = NULL);
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