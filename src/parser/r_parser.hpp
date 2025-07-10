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

#ifndef R_PARSER_HPP
#define R_PARSER_HPP

#include <vector>
#include "../../src/tokens/t_tokens.hpp"
#include "../../src/includes/ast.hpp"
#include "../../src/lexer/lex_types.hpp"


namespace Rythin {

    class Parser {
        private:
        int position;
        std::vector<Tokens> tokens;
        public:
        inline static std::vector<std::string> codes;
        Parser(std::vector<Tokens> tokens) : position(0), tokens(tokens){}
        Tokens current();
        Tokens peek(int offset); // Added: Peeks at a token without consuming it.
        Tokens consume(TokensTypes tk);
        std::vector<ASTPtr> Parse();
        bool check(TokensTypes tk);
        bool lookAhead(TokensTypes tk);
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
        ASTPtr ParseVarCall();
        ASTPtr ParseCharseqValues();

        // New functions for arithmetic expression parsing with precedence
        ASTPtr ParsePrimaryExpression();      // Handles numbers, identifiers, and parenthesized expressions
        ASTPtr ParseMultiplicativeExpression(); // Handles multiplication, division, modulo
        ASTPtr ParseAdditiveExpression();     // Handles addition, subtraction

        std::string getCode();

        ASTPtr ParseByteVal();
        ASTPtr ParseNumeralExpression();
        //ASTPtr Parse
        ASTPtr ParseIntVal(); /// This will now call ParseAdditiveExpression to handle full arithmetic expressions

        bool isConditionOperator(TokensTypes type) {
            return type == TokensTypes::TOKEN_LESS_THAN || // < 
                   type == TokensTypes::TOKEN_GREATER_THAN || // >
                   type == TokensTypes::TOKEN_EQUAL ||  // ==
                   type == TokensTypes::TOKEN_NOT_EQUAL ||  //!=\
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
