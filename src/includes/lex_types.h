#include <string>

#ifndef LEX_TYPES_H
#define LEX_TYPES_H

enum class TokensTypes {
    TOKEN_EOF, //end of file or end of line (eol)
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING_LITERAL,
    TOKEN_COLON,
    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_ARROW_SET,
    TOKEN_ATTR_MINUS,
    TOKEN_MINUS,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_PLUSPLUS,
    TOKEN_ATTR_PLUS,
    TOKEN_PLUS,
    TOKEN_DOT,
    TOKEN_ATTR_DIVIDE,
    TOKEN_DIVIDE,
    TOKEN_LOGICAL_AND,
    TOKEN_REF,
    TOKEN_DEREF,
    TOKEN_COMMA,
    TOKEN_ATTR_MULTIPLY,
    TOKEN_MULTIPLY,
    TOKEN_SHIFT_RIGHT,
    TOKEN_GREATER_EQUAL,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_EQUAL,
    TOKEN_SHIFT_LEFT,
    TOKEN_LESS_THAN,
    TOKEN_LOGICAL_OR,
    TOKEN_BIT_OR,
    TOKEN_MODULO,
    TOKEN_BIT_NOT,
    TOKEN_BIT_XOR,
    TOKEN_USING,
    TOKEN_CONST,
    TOKEN_FINISH,
    TOKEN_DEF,
    TOKEN_LET,
    TOKEN_FROM,
    TOKEN_GET,
    TOKEN_LOOP,
    TOKEN_IN,
    TOKEN_STOP,
    TOKEN_CONTINUE,
    TOKEN_IF,
    TOKEN_BUT,
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_RETURN,
    TOKEN_ALLOC,
    TOKEN_FLUSH,
    TOKEN_FWRITE,
    TOKEN_FREAD,
    TOKEN_MKDIR,
    TOKEN_LEN,
    TOKEN_BOOL,
    TOKEN_STR,
    TOKEN_INT_32, //i32 bits
    TOKEN_INT_64, //i64 bits
    TOKEN_FUNC,
    TOKEN_CHAR,
    TOKEN_FLOAT_32, //f32
    TOKEN_FLOAT_IND,    //f
    TOKEN_FLOAT_64, //f64
    TOKEN_BYTES,
    TOKEN_OBJECT,
    TOKEN_PRINT,
    TOKEN_NEW_LINE,
    TOKEN_PRINT_NEW_LINE,
    TOKEN_PRINT_ERROR,
    TOKEN_PRINT_ERROR_LOG,
    TOKEN_CINPUT,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NIL,
    TOKEN_HAS
};

#endif