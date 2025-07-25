// Copyright (C) 2025 Rafael de Sousa (el-rafa-dev)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

// lex_types.cpp
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <limits.h>
#include <queue>

// local includes
#include "../../src/lexer/r_lex.hpp"
#include "../../src/tokens/t_tokens.hpp"
#include "../../src/lexer/lex_types.hpp"
#include "../../src/includes/rexcept.hpp"
#include "../../src/includes/log.hpp"
#include "../../src/includes/ast.hpp"

using namespace Log;
using namespace std;

std::queue<Tokens> token_buffer;

namespace Rythin
{
    Lexer::Lexer(std::string input) : code_input(input), position(0), line(1), column(0)
    {
        current_input = (input.length() > 0) ? input[position] : '\0';
    }

    Tokens Lexer::next_tk()
    {
        skip_withspace();
        if (std::isalpha(current_input) || current_input == '_')
        {
            return identifier();
        }
        else if (current_input == '"')
        {
            return stringLiteral();
        }
        else if (std::isdigit(current_input) == true || current_input == '.')
        {
            bool isFloat = false;
            bool isDouble = false;

            std::string numberStr;

            if (current_input == '.')
            {
                numberStr += current_input;
                advance_tk();

                std::string fractional = digits();
                if (fractional.empty())
                {
                    LogErrors::getInstance().addError("Invalid float/double format", 23, line, column);
                    //throw Excepts::SyntaxException("\f\fInvalid float/double format at line: " + line);
                }

                numberStr += fractional;
                isDouble = true;

                if (current_input == 'f')
                {
                    isFloat = true;
                    isDouble = false;
                    advance_tk();
                }

                return Tokens(isFloat ? TokensTypes::TOKEN_FLOAT_32 : TokensTypes::TOKEN_FLOAT_64, numberStr, line, column);
            }

            numberStr += digits();

            if (current_input == '.')
            {
                numberStr += current_input;
                advance_tk();

                std::string fractional = digits();
                if (fractional.empty())
                {
                    LogErrors::getInstance().addError("Invalid float/double format", 23, line, column);
                    //throw std::runtime_error("Invalid float/double format");
                }

                numberStr += fractional;
                isDouble = true;
            }

            if (current_input == 'f')
            {
                isFloat = true;
                isDouble = false;
                advance_tk();
            }

            if (isFloat)
                return Tokens(TokensTypes::TOKEN_FLOAT_32, numberStr, line, column);
            else if (isDouble)
                return Tokens(TokensTypes::TOKEN_FLOAT_64, numberStr, line, column);
            else
                return Tokens(TokensTypes::TOKEN_INT_32, numberStr, line, column);
        }
        else
        {
            switch (current_input)
            {
            case '\0':
                return Tokens(TokensTypes::TOKEN_EOF, std::string("\0"), line, column);
            case ';':
                advance_tk();

                while (current_input != '\0' && current_input != '\n')
                {
                    advance_tk();
                }
                return next_tk();
            case '#':

            {
                while (current_input != '\0' and current_input != '\n')
                {
                    advance_tk();
                }
                return next_tk();
            }

            case ':':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ASSIGN, ":=", line, column);

                }
                return Tokens(TokensTypes::TOKEN_COLON, ":", line, column);
            case '=':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_EQUAL, "==", line, column);
                }
            case '!':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_NOT_EQUAL, "!=", line, column);

                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_NOT_EQUAL, "!", line, column);

                }
            case '-':
                advance_tk();
                if (current_input == '>')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ARROW_SET, "->", line, column);

                }
                else if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_MINUS, "-=", line, column);

                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_MINUS, "-", line, column);

                }
            case '[':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_LBRACKET, "[", line, column);

            case ']':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_RBRACKET, "]", line, column);

            case '(':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_LPAREN, "(", line, column);

            case ')':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_RPAREN, ")", line, column);

            case '+':
                advance_tk();
                if (current_input == '+')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_PLUSPLUS, "++", line, column);

                }
                else if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_PLUS, "+=", line, column);

                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_PLUS, "+", line, column);

                }
            case '.':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_DOT, ".", line, column);

            case '/':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_DIVIDE, "/=", line, column);

                }
                return Tokens(TokensTypes::TOKEN_DIVIDE, "/", line, column);

            case '&':
                advance_tk();
                if (current_input == '&')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_LOGICAL_AND, "&&", line, column);

                }
                return Tokens(TokensTypes::TOKEN_REF, "&", line, column);

            case '$':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_DEREF, "$", line, column);

            case ',':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_COMMA, ",", line, column);

            case '*':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_MULTIPLY, "*=", line, column);

                }
                return Tokens(TokensTypes::TOKEN_MULTIPLY, "*", line, column);

            case '>':
                advance_tk();
                if (current_input == '>')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_SHIFT_RIGHT, ">>", line, column);

                }
                else if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_GREATER_EQUAL, ">=", line, column);

                }
                else
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_GREATER_THAN, ">", line, column);

                }
            case '<':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_LESS_EQUAL, "<=", line, column);

                }
                else if (current_input == '<')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_SHIFT_LEFT, "<<", line, column);

                }
                return Tokens(TokensTypes::TOKEN_LESS_THAN, "<", line, column);

            case '|':
                advance_tk();
                if (current_input == '|')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_LOGICAL_OR, "||", line, column);

                }
                return Tokens(TokensTypes::TOKEN_BIT_OR, "|", line, column);

            case '%':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_MODULO, "%", line, column);

            case '~':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_BIT_NOT, "~", line, column);

            case '^':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_BIT_XOR, "^", line, column);
            default:
                LogErrors::getInstance().addError("Unexpected character: '" + to_string(current_input) + "'", 76, line, column);
                LogErrors::getInstance().printAll();
                exit(76);
            }
        }
    }

    void Lexer::advance_tk(bool isComment)
    {
        if (current_input == '\n')
        {
            line++;
            column = 0;
        }
        else
        {
            column++;
        }
        position++;
        current_input = (position < code_input.length()) ? code_input[position] : '\0';
    }

    void Lexer::skip_withspace()
    {
        while (std::isspace(static_cast<unsigned char>(current_input)))
        {
            advance_tk();
        }
    }

    Tokens Lexer::identifier()
    {
        int start = position;
        while (std::isalnum(current_input) || current_input == '_' || current_input == '-')
        {
            advance_tk();
        }

        std::string value = code_input.substr(start, position - start);
        static const std::unordered_map<std::string, TokensTypes> keywords = {
            {"using", TokensTypes::TOKEN_USING},
            {"const", TokensTypes::TOKEN_CONST},
            {"finish", TokensTypes::TOKEN_FINISH},
            {"def", TokensTypes::TOKEN_DEF},
            {"let", TokensTypes::TOKEN_LET},
            {"from", TokensTypes::TOKEN_FROM},
            {"get", TokensTypes::TOKEN_GET},
            {"loop", TokensTypes::TOKEN_LOOP},
            {"in", TokensTypes::TOKEN_IN},
            {"and", TokensTypes::TOKEN_LOGICAL_AND},
            {"or", TokensTypes::TOKEN_LOGICAL_OR},
            {"stop", TokensTypes::TOKEN_STOP},
            {"continue", TokensTypes::TOKEN_CONTINUE},
            {"if", TokensTypes::TOKEN_IF},
            {"but", TokensTypes::TOKEN_BUT},
            {"try", TokensTypes::TOKEN_TRY},
            {"catch", TokensTypes::TOKEN_CATCH},
            {"return", TokensTypes::TOKEN_RETURN},
            {"alloc", TokensTypes::TOKEN_ALLOC},
            {"flush", TokensTypes::TOKEN_FLUSH},
            {"cinput", TokensTypes::TOKEN_CINPUT},
            {"fwrite", TokensTypes::TOKEN_FWRITE},
            {"fread", TokensTypes::TOKEN_FREAD},
            {"mkdir", TokensTypes::TOKEN_MKDIR},
            {"len", TokensTypes::TOKEN_LEN},
            {"int32", TokensTypes::TOKEN_INT_32},
            {"int64", TokensTypes::TOKEN_INT_64},
            {"f", TokensTypes::TOKEN_FLOAT_IND},
            {"bool", TokensTypes::TOKEN_BOOL},
            {"charseq", TokensTypes::TOKEN_CHARSEQ},
            {"obj", TokensTypes::TOKEN_OBJECT},
            {"func", TokensTypes::TOKEN_FUNC},
            {"float32", TokensTypes::TOKEN_FLOAT_32},
            {"float64", TokensTypes::TOKEN_FLOAT_64},
            {"byte", TokensTypes::TOKEN_BYTES},
            {"print", TokensTypes::TOKEN_PRINT},
            {"printnl", TokensTypes::TOKEN_PRINT_NEW_LINE},
            {"printe", TokensTypes::TOKEN_PRINT_ERROR},
            {"true", TokensTypes::TOKEN_TRUE},
            {"false", TokensTypes::TOKEN_FALSE},
            {"parallel", TokensTypes::TOKEN_PARALLEL},
            {"nil", TokensTypes::TOKEN_NIL},
            {"has", TokensTypes::TOKEN_HAS}};

        if (keywords.count(value))
        {
            return Tokens(keywords.at(value), value, line, column);
        }
        return Tokens(TokensTypes::TOKEN_IDENTIFIER, value, line, column);
    }

    Tokens Lexer::stringLiteral()
    {
        advance_tk(); // Skip opening quote
        int start = position;
        std::string ivalue = "";
        while (current_input != '"' && current_input != '\0')
        {
            if (current_input == '\\')
            {
                advance_tk();
                if (current_input == 'n') // new line
                {
                    ivalue += '\n';
                    advance_tk();
                }
                else if (current_input == 't') // horizontal tab
                {
                    ivalue += '\t';
                    advance_tk();
                }
                else if (current_input == '0') // null character
                {
                    ivalue += '\0';
                    advance_tk();
                }
                else if (current_input == 'v') // vertical tab
                {
                    ivalue += '\v';
                    advance_tk();
                }
                else if (current_input == 'r') // carriage return
                {
                    ivalue += '\r';
                    advance_tk();
                }
                else if (current_input == 'b') // backspace
                {
                    ivalue += '\b';
                    advance_tk();
                }
                else if (current_input == 'f') // form feed
                {
                    ivalue += '\f';
                    advance_tk();
                }
                else if (current_input == 'a') // alarm
                {
                    ivalue += '\a';
                    advance_tk();
                }
                else if (current_input == '"') // double quote
                {
                    ivalue += '"';
                    advance_tk();
                }
                else if (current_input == '\\') // backslash
                {
                    ivalue += '\\';
                    advance_tk();
                }
                else
                {
                    // LogErrors
                    LogErrors::getInstance().addError("Unknown escape sequence \\" + current_input, 34, line, column);
                    //std::cerr << "Unknown escape sequence \\" << current_input << " at line " << line << ", column " << column << std::endl;
                    continue;
                    //throw std::runtime_error("Unknown escape sequence");
                }
            }
            else if (current_input == '$' && peekNextChar() == '[')
            {
                advance_tk(); // Skip $
                advance_tk(); // Skip [
                ivalue += processInterpolation();
            }
            else
            {
                ivalue += current_input;
                advance_tk();
            }
        }
        if (current_input == '\0')
        {
            LogErrors::getInstance().addError("Unterminated/unclosed string literal", 14, line, column);
        }
        advance_tk(); // skipping closing quotes
        return Tokens(TokensTypes::TOKEN_STRING_LITERAL, ivalue, line, column);
    }

    char Lexer::peekNextChar() const
    {
        int nextPosition = position + 1;
        return (nextPosition < code_input.length()) ? code_input[nextPosition] : '\0';
    }

    std::string Lexer::processInterpolation()
    {
        int start = position;
        while (current_input != ']' && current_input != '\0')
        {
            advance_tk();
        }
        if (current_input == ']')
        {
            auto val = code_input.substr(start, position - start);
            advance_tk(); // Skip ]
            return val;
        }
        else
        {
            LogErrors::getInstance().addError("Unterminated interpolation", 14, line, column);
            return nullptr;
        }
    }

    std::string Lexer::digits()
    {
        std::string res = "";
        while (current_input != '\0' && std::isdigit(current_input))
        {
            res += current_input;
            advance_tk();
        }
        return res;
    }
}