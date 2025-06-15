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
        public:
        Parser(std::vector<Tokens> tokens) : position(0), tokens(tokens){}
        Tokens current();
        Tokens consume(TokensTypes tk);
        std::vector<ASTPtr> Parse();
        bool check(TokensTypes tk);
        //private functions
        private:
        ASTPtr ParsePrint();
        ASTPtr ParsePrintE();
        ASTPtr ParsePrintNl();
        ASTPtr ParseIfStatement();
        ASTPtr ParseIfExpressions();
        ASTPtr ParseBlock();
        ASTPtr ParseLoopExpression();
        ASTPtr ParseDeclarations();
        ASTPtr ParseExpression(TokensTypes types);
        ASTPtr ParseFuncDeclaration();
        ASTPtr ParseFuncExpressions();
        ASTPtr ParseCinput();
        ASTPtr ParseLoopCondition();
        ASTPtr ParseLoopCond();
        ASTPtr ParsePositiveVals(TokensTypes curr);
        ASTPtr ParseNegativeVals(TokensTypes curr);
        ASTPtr ParseVarDeclaration();
        ASTPtr ParseFactor();
        ASTPtr ParseTerm();
        ASTPtr ParseExp();
        ASTPtr ParseNumeralExpression();
        //ASTPtr Parse
        ASTPtr ParseIntVal(); /// to parse the int or arithmetic or others types of expressions like division or addition values

        bool isConditionOperator(TokensTypes type) {
            return type == TokensTypes::TOKEN_LESS_THAN || // < 
                   type == TokensTypes::TOKEN_GREATER_THAN || // >
                   type == TokensTypes::TOKEN_EQUAL ||  // ==
                   type == TokensTypes::TOKEN_NOT_EQUAL ||  //!=
                   type == TokensTypes::TOKEN_LESS_EQUAL || // <=
                   type == TokensTypes::TOKEN_GREATER_EQUAL; // >=
        }

        bool isTrueOrFalseOperator(TokensTypes tk) {
            return tk == TokensTypes::TOKEN_TRUE ||
                   tk == TokensTypes::TOKEN_FALSE ||
                   tk == TokensTypes::TOKEN_IDENTIFIER;
        }

        bool isBinaryOperator(TokensTypes tk) {
            return tk == TokensTypes::TOKEN_PLUS || tk == TokensTypes::TOKEN_MINUS || tk == TokensTypes::TOKEN_DIVIDE || tk == TokensTypes::TOKEN_MULTIPLY || tk == TokensTypes::TOKEN_MODULO || tk == TokensTypes::TOKEN_BIT_XOR;
        }

        

        bool isDivisor(TokensTypes tk) {
            return tk == TokensTypes::TOKEN_LOGICAL_OR ||
                   tk == TokensTypes::TOKEN_LOGICAL_AND;
        }
    };
}

#endif // R_PARSER_H