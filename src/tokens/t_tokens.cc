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

#include "t_tokens.hpp"
#include <string>
#include <cctype>
#include <iostream>

Tokens::Tokens(TokensTypes type, std::string value, int line, int column)
{
    this->type = type;
    this->value = value;
    this->line = line;
    this->column = column;
}

std::string Tokens::tokenTypeToString(TokensTypes type)
{
    switch (type)
    {
    case TokensTypes::TOKEN_EOF:
        return "end of file";
    case TokensTypes::TOKEN_IDENTIFIER:
        return "identifier";
    case TokensTypes::TOKEN_NUMBER:
        return "number";
    case TokensTypes::TOKEN_STRING_LITERAL:
        return "string literal";
    case TokensTypes::TOKEN_COLON:
        return ":";
    case TokensTypes::TOKEN_FLOAT_IND:
        return "f";
    case TokensTypes::TOKEN_ASSIGN:
        return ":=";
    case TokensTypes::TOKEN_EQUAL:
        return "==";
    case TokensTypes::TOKEN_NOT_EQUAL:
        return "!=";
    case TokensTypes::TOKEN_ARROW_SET:
        return "->";
    case TokensTypes::TOKEN_ATTR_MINUS:
        return "-=";
    case TokensTypes::TOKEN_MINUS:
        return "-";
    case TokensTypes::TOKEN_LBRACKET:
        return "[";
    case TokensTypes::TOKEN_RBRACKET:
        return "]";
    case TokensTypes::TOKEN_LPAREN:
        return "(";
    case TokensTypes::TOKEN_RPAREN:
        return ")";
    case TokensTypes::TOKEN_PLUSPLUS:
        return "++";
    case TokensTypes::TOKEN_ATTR_PLUS:
        return "+=";
    case TokensTypes::TOKEN_PLUS:
        return "+";
    case TokensTypes::TOKEN_DOT:
        return ".";
    case TokensTypes::TOKEN_ATTR_DIVIDE:
        return "/=";
    case TokensTypes::TOKEN_DIVIDE:
        return "/";
    case TokensTypes::TOKEN_LOGICAL_AND:
        return "&&";
    case TokensTypes::TOKEN_REF:
        return "&";
    case TokensTypes::TOKEN_DEREF:
        return "$";
    case TokensTypes::TOKEN_COMMA:
        return ",";
    case TokensTypes::TOKEN_ATTR_MULTIPLY:
        return "*=";
    case TokensTypes::TOKEN_MULTIPLY:
        return "*";
    case TokensTypes::TOKEN_SHIFT_RIGHT:
        return ">>";
    case TokensTypes::TOKEN_GREATER_EQUAL:
        return ">=";
    case TokensTypes::TOKEN_GREATER_THAN:
        return ">";
    case TokensTypes::TOKEN_LESS_EQUAL:
        return "<=";
    case TokensTypes::TOKEN_SHIFT_LEFT:
        return "<<";
    case TokensTypes::TOKEN_LESS_THAN:
        return "<";
    case TokensTypes::TOKEN_LOGICAL_OR:
        return "||";
    case TokensTypes::TOKEN_BIT_OR:
        return "|";
    case TokensTypes::TOKEN_MODULO:
        return "%";
    case TokensTypes::TOKEN_BIT_NOT:
        return "~";
    case TokensTypes::TOKEN_BIT_XOR:
        return "^";

    // keywords
    case TokensTypes::TOKEN_PARALLEL:
        return "parallel";
    case TokensTypes::TOKEN_USING:
        return "using";
    case TokensTypes::TOKEN_CONST:
        return "const";
    case TokensTypes::TOKEN_FINISH:
        return "finish";
    case TokensTypes::TOKEN_DEF:
        return "def";
    case TokensTypes::TOKEN_LET:
        return "let";
    case TokensTypes::TOKEN_FROM:
        return "from";
    case TokensTypes::TOKEN_GET:
        return "get";
    case TokensTypes::TOKEN_LOOP:
        return "loop";
    case TokensTypes::TOKEN_IN:
        return "in";
    case TokensTypes::TOKEN_STOP:
        return "stop";
    case TokensTypes::TOKEN_CONTINUE:
        return "continue";
    case TokensTypes::TOKEN_IF:
        return "if";
    case TokensTypes::TOKEN_BUT:
        return "but";
    case TokensTypes::TOKEN_TRY:
        return "try";
    case TokensTypes::TOKEN_CATCH:
        return "catch";
    case TokensTypes::TOKEN_RETURN:
        return "return";
    case TokensTypes::TOKEN_ALLOC:
        return "alloc";
    case TokensTypes::TOKEN_FLUSH:
        return "flush";
    case TokensTypes::TOKEN_FWRITE:
        return "fwrite";
    case TokensTypes::TOKEN_FREAD:
        return "fread";
    case TokensTypes::TOKEN_MKDIR:
        return "mkdir";
    case TokensTypes::TOKEN_LEN:
        return "len";
    case TokensTypes::TOKEN_INT_32:
        return "int32";
    case TokensTypes::TOKEN_INT_64:
        return "int64";
    case TokensTypes::TOKEN_BOOL:
        return "bool";
    case TokensTypes::TOKEN_FUNC:
        return "func";
    case TokensTypes::TOKEN_CHARSEQ:
        return "char";
    case TokensTypes::TOKEN_FLOAT_32:
        return "float32";
    case TokensTypes::TOKEN_FLOAT_64:
        return "float64";
    case TokensTypes::TOKEN_OBJECT:
        return "obj";
    case TokensTypes::TOKEN_BYTES:
        return "byte";
    case TokensTypes::TOKEN_PRINT:
        return "print";
    case TokensTypes::TOKEN_PRINT_NEW_LINE:
        return "printnl";
    case TokensTypes::TOKEN_PRINT_ERROR:
        return "printe";
    case TokensTypes::TOKEN_CINPUT:
        return "cinput";
    case TokensTypes::TOKEN_TRUE:
        return "true";
    case TokensTypes::TOKEN_FALSE:
        return "false";
    case TokensTypes::TOKEN_HAS:
        return "has";
    case TokensTypes::TOKEN_NIL:
        return "nil";
    default:
        return "Unknown Token\n";
    }
}