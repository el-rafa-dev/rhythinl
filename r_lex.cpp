// lex_types.cpp
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <limits.h>

// local includes
#include "r_lex.h"
#include "t_tokens.h"
#include "lex_types.h"
#include "rexcept.h"
#include "log.h"
#include "ast.h"

using namespace Log;
using namespace std;

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
                    LogErrors::getInstance().addError("Invalid float/double format at line " + to_string(line) + " column " + to_string(column), 23);
                    throw Excepts::SyntaxException("\f\fInvalid float/double format at line: " + line);
                }

                numberStr += fractional;
                isDouble = true;

                if (current_input == 'f')
                {
                    isFloat = true;
                    isDouble = false;
                    advance_tk();
                }

                return Tokens(isFloat ? TokensTypes::TOKEN_FLOAT : TokensTypes::TOKEN_DOUBLE, numberStr, line, column);
            }

            numberStr += digits();

            if (current_input == '.')
            {
                numberStr += current_input;
                advance_tk();

                std::string fractional = digits();
                if (fractional.empty())
                {
                    LogErrors::getInstance().addError("Invalid float/double format at line " + to_string(line) + " column " + to_string(column), 23);
                    throw std::runtime_error("Invalid float/double format");
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
                return Tokens(TokensTypes::TOKEN_FLOAT, numberStr, line, column);
            else if (isDouble)
                return Tokens(TokensTypes::TOKEN_DOUBLE, numberStr, line, column);
            else
                return Tokens(TokensTypes::TOKEN_INT, numberStr, line, column);
        }
        else
        {
            switch (current_input)
            {
            case '\0':
                return Tokens(TokensTypes::TOKEN_EOF, std::string("\0"), line, column);
                break;
            case ';':
                advance_tk();
                
                while (current_input != '\0' && current_input != '\n')
                {
                    advance_tk();
                }
                return next_tk();
            
            case '#':
                advance_tk();
                while (current_input != '\0' && current_input != '\n') {
                    advance_tk();
                }
                return next_tk();

            case ':':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_COLON, ":", line, column);
            case '=':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_EQUAL, "==", line, column);
                    break;
                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_ASSIGN, "=", line, column);
                    break;
                }
            case '!':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_NOT_EQUAL, "!=", line, column);
                    break;
                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_NOT_EQUAL, "!", line, column);
                    break;
                }
            case '-':
                advance_tk();
                if (current_input == '>')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ARROW_SET, "->", line, column);
                    break;
                }
                else if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_MINUS, "-=", line, column);
                    break;
                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_MINUS, "-", line, column);
                    break;
                }
            case '[':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_LBRACKET, "[", line, column);
                break;
            case ']':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_RBRACKET, "]", line, column);
                break;
            case '(':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_LPAREN, "(", line, column);
                break;
            case ')':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_RPAREN, ")", line, column);
                break;
            case '+':
                advance_tk();
                if (current_input == '+')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_PLUSPLUS, "++", line, column);
                    break;
                }
                else if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_PLUS, "+=", line, column);
                    break;
                }
                else
                {
                    return Tokens(TokensTypes::TOKEN_PLUS, "+", line, column);
                    break;
                }
            case '.':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_DOT, ".", line, column);
                break;
            case '/':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_DIVIDE, "/=", line, column);
                    break;
                }
                return Tokens(TokensTypes::TOKEN_DIVIDE, "/", line, column);
                break;
            case '&':
                advance_tk();
                if (current_input == '&')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_LOGICAL_AND, "&&", line, column);
                    break;
                }
                return Tokens(TokensTypes::TOKEN_REF, "&", line, column);
                break;
            case '$':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_DEREF, "$", line, column);
                break;
            case ',':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_COMMA, ",", line, column);
                break;
            case '*':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_ATTR_MULTIPLY, "*=", line, column);
                    break;
                }
                return Tokens(TokensTypes::TOKEN_MULTIPLY, "*", line, column);
                break;
            case '>':
                advance_tk();
                if (current_input == '>')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_SHIFT_RIGHT, ">>", line, column);
                    break;
                }
                else if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_GREATER_EQUAL, ">=", line, column);
                    break;
                }
                else
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_GREATER_THAN, ">", line, column);
                    break;
                }
            case '<':
                advance_tk();
                if (current_input == '=')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_LESS_EQUAL, "<=", line, column);
                    break;
                }
                else if (current_input == '<')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_SHIFT_LEFT, "<<", line, column);
                    break;
                }
                return Tokens(TokensTypes::TOKEN_LESS_THAN, "<", line, column);
                break;
            case '|':
                advance_tk();
                if (current_input == '|')
                {
                    advance_tk();
                    return Tokens(TokensTypes::TOKEN_LOGICAL_OR, "||", line, column);
                    break;
                }
                return Tokens(TokensTypes::TOKEN_BIT_OR, "|", line, column);
                break;
            case '%':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_MODULO, "%", line, column);
                break;
            case '~':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_BIT_NOT, "~", line, column);
                break;
            case '^':
                advance_tk();
                return Tokens(TokensTypes::TOKEN_BIT_XOR, "^", line, column);
                break;
            default:
                std::cerr << "Unexpected character: '" << current_input << "' at line " << line << " column " << column << std::endl;
                throw std::runtime_error("Unexpected character");
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
            {"int", TokensTypes::TOKEN_INT},
            {"f", TokensTypes::TOKEN_FLOAT_IND},
            {"bool", TokensTypes::TOKEN_BOOL},
            {"str", TokensTypes::TOKEN_STR},
            {"lint", TokensTypes::TOKEN_LONG_INT},
            {"obj", TokensTypes::TOKEN_OBJECT},
            {"func", TokensTypes::TOKEN_FUNC},
            {"char", TokensTypes::TOKEN_CHAR},
            {"float", TokensTypes::TOKEN_FLOAT},
            {"double", TokensTypes::TOKEN_DOUBLE},
            {"byte", TokensTypes::TOKEN_BYTES},
            {"print", TokensTypes::TOKEN_PRINT},
            {"printnl", TokensTypes::TOKEN_PRINT_NEW_LINE},
            {"printe", TokensTypes::TOKEN_PRINT_ERROR},
            {"true", TokensTypes::TOKEN_TRUE},
            {"false", TokensTypes::TOKEN_FALSE},
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
                    std::cerr << "Unknown escape sequence \\" << current_input << " at line " << line << ", column " << column << std::endl;
                    throw std::runtime_error("Unknown escape sequence");
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
            std::cerr << "[Error]: Unterminated string literal at line " << line << std::endl;
            throw Excepts::SyntaxException("Unterminated string literal");
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
            auto inter_node = std::make_shared<InterpolationNode>();
            inter_node->var_name = code_input.substr(start, position - start);
            advance_tk(); // Skip ]
            return inter_node->val;
        }
        else
        {
            std::cerr << "[Error]Unterminated interpolation at line " << line << " Column " << column << std::endl;
            throw std::runtime_error("Unterminated interpolation");
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