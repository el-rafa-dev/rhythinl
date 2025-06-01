#ifndef R_PARSER_H
#define R_PARSER_H

#include "vector"
#include "t_tokens.h"
#include "ast.h"
#include "lex_types.h"
#include "chunk.h"


namespace Rythin {

    class Parser {
        private:
        int position;
        std::vector<Tokens> tokens;
        std::string current_file;
        public:
        Parser(std::vector<Tokens> tokens, std::string current_file);
        Tokens current();
        Tokens consume(TokensTypes tk);
        std::vector<ASTPtr> Parse();
        bool check(TokensTypes tk);
        //private functions
        private:
        Rythin::Chunk ch;
        ASTPtr ParsePrint();
        ASTPtr ParsePrintE();
        ASTPtr ParsePrintNl();
        ASTPtr ParsePrintEL();
        ASTPtr ParseIfStatement();
        ASTPtr ParsePrimary();
        int GetPrecedence(TokensTypes tk);
        ASTPtr ParseLoop();
        ASTPtr ParseBlock();
        ASTPtr ParseLoopExpression();
        ASTPtr ParseDeclarations();
        ASTPtr ParseExpression();
        ASTPtr ParseVarExpression();
        ASTPtr ParseDefinitions();

        ASTPtr ParseVarDeclaration();

        bool isBinOperator(TokensTypes type) {
            return type == TokensTypes::TOKEN_PLUS ||
                   type == TokensTypes::TOKEN_MINUS ||
                   type == TokensTypes::TOKEN_MULTIPLY ||
                   type == TokensTypes::TOKEN_DIVIDE ||
                   type == TokensTypes::TOKEN_LESS_THAN ||
                   type == TokensTypes::TOKEN_GREATER_THAN ||
                   type == TokensTypes::TOKEN_EQUAL ||
                   type == TokensTypes::TOKEN_NOT_EQUAL ||
                   type == TokensTypes::TOKEN_LESS_EQUAL ||
                   type == TokensTypes::TOKEN_GREATER_EQUAL;
        }

        bool isOperator(TokensTypes tk) {
            return tk == TokensTypes::TOKEN_TRUE ||
                   tk == TokensTypes::TOKEN_FALSE ||
                   tk == TokensTypes::TOKEN_IDENTIFIER;
        }
    };
}

#endif // R_PARSER_H