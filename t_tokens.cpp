#include "t_tokens.h"
#include <string>
#include <cctype>

// Remova o namespace Rythin aqui!

Tokens::Tokens(TokensTypes type, std::string value, int line, int column)
{
    this->type = type;
    this->value = value; // Use o nome correto do membro: value
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
        return "=";
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
    case TokensTypes::TOKEN_INPUT:
        return "input";
    case TokensTypes::TOKEN_FWRITE:
        return "fwrite";
    case TokensTypes::TOKEN_FREAD:
        return "fread";
    case TokensTypes::TOKEN_MKDIR:
        return "mkdir";
    case TokensTypes::TOKEN_LEN:
        return "len";
    case TokensTypes::TOKEN_INT:
        return "int";
    case TokensTypes::TOKEN_BOOL:
        return "bool";
    case TokensTypes::TOKEN_STR:
        return "str";
    case TokensTypes::TOKEN_LONG_INT:
        return "lint";
    case TokensTypes::TOKEN_FUNC:
        return "func";
    case TokensTypes::TOKEN_CHAR:
        return "char";
    case TokensTypes::TOKEN_FLOAT:
        return "float";
    case TokensTypes::TOKEN_DOUBLE:
        return "double";
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
    case TokensTypes::TOKEN_PRINT_ERROR_LOG:
        return "printel";
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