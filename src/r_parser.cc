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

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <optional>
#include <math.h>
#include <iomanip>

// local includes
#include "../src/includes/ast.hpp"
#include "../src/includes/t_tokens.hpp"
#include "../src/includes/r_parser.hpp"
#include "../src/includes/r_opcodes.hpp"
#include "../src/includes/rexcept.hpp"
#include "../src/includes/log.hpp"

// using namespace std;
using namespace Log;

namespace Rythin
{

    Tokens Parser::current()
    {
        if (position >= 0 && position < tokens.size())
        {
            return tokens[position];
        }
        return tokens[tokens.size() - 1];
    }

    Tokens Parser::consume(TokensTypes tk)
    {
        codes.push_back(current().value);
        // std::cout << "Code: " <<  << std::endl;

        if (current().type == TokensTypes::TOKEN_EOF)
        {
            // checa se o token atual é o EOF (end of file), se for retorna o erro abaixo
            LogErrors::getInstance().addError("Expected a statement but reached the end of file and left early.... Are you forget anything?", 1, 0, 0);
            LogErrors::getInstance().printAll();
            exit(1);
        }

        if (current().type != tk)
        {
            // check if the current type matches the expected token
            LogErrors::getInstance().addError("Expected " + Tokens::tokenTypeToString(tk) + " token but got: " + Tokens::tokenTypeToString(current().type), 4, current().line, current().column);
        }

        position++;
        return tokens[position - 1];
    }

    std::vector<ASTPtr> Parser::Parse()
    {

        std::vector<ASTPtr> node;
        while (current().type != TokensTypes::TOKEN_EOF)
        {
            node.push_back(ParseDeclarations());
        }
        return node;
    }

    // look the if current type is the expected token
    bool Parser::check(TokensTypes tk)
    {
        return current().type == tk;
    }

    bool Parser::lookAhead(TokensTypes tk)
    {
        int oldpos = position;
        while (!check(tk))
        {
            consume(current().type);
            if (check(tk))
            {
                position = oldpos;
                // return true if is the expected tk
                return true;
            }
            else
            {
                position = oldpos;
                return false;
            }
        }
        return false;
    }

    ASTPtr Parser::ParseDeclarations()
    {
        switch (current().type)
        {
        case TokensTypes::TOKEN_CINPUT:
            return ParseCinput();
        case TokensTypes::TOKEN_IF:
            return ParseIfStatement();
        case TokensTypes::TOKEN_LOOP:
        {
            int oldpos = position;
            while (!check(TokensTypes::TOKEN_COLON))
            {
                consume(current().type);
                std::cout << "Debug Current(): " << Tokens::tokenTypeToString(current().type) << "\n";
                if (check(TokensTypes::TOKEN_COLON))
                {
                    position = oldpos;
                    std::cout << "Contains colon? true" << "\n";
                    return ParseLoopExpression();
                } else /*if (check(TokensTypes::TOKEN_IDENTIFIER))*/ {
                    std::cout << "Contains colon? false" << "\n";
                    //std::cout << "Debug Current(): " << Tokens::tokenTypeToString(current().type) << "\n";
                    position = oldpos;
                    return ParseLoopCond();
                }
            }
        }

        case TokensTypes::TOKEN_PRINT:
            return ParsePrint();
        case TokensTypes::TOKEN_PRINT_ERROR:
            return ParsePrintE();
        case TokensTypes::TOKEN_PRINT_NEW_LINE:
            return ParsePrintNl();
        case TokensTypes::TOKEN_STRING_LITERAL:
            consume(TokensTypes::TOKEN_STRING_LITERAL);
            return nullptr;
        case TokensTypes::TOKEN_DEF:
        {
            // stores the current position on another int
            int pos = position;
            // look ahead if have the assign (=) token
            // if not, is check if have the left paren ( token
            // the ( token defines thats the variable is a function regardless of type
            while (!check(TokensTypes::TOKEN_ASSIGN))
            {
                consume(current().type);
                //std::cout << "Debug Current(): " << Tokens::tokenTypeToString(current().type) << "\n";
                if (check(TokensTypes::TOKEN_ASSIGN))
                {
                    // sets the position to the old position - controlled backtracking
                    position = pos;
                    return ParseVarDeclaration();
                }
                else if (check(TokensTypes::TOKEN_LPAREN))
                {
                    // sets the position to the old position again - controlled backtracking
                    position = pos;
                    return ParseFuncDeclaration();
                }
            }
        }

        default:
            // if the current type don't have the valid statements keywords,
            // throw a compilation exception
            LogErrors::getInstance().addError("Invalid statement/keyword '" + Tokens::tokenTypeToString(current().type) + "'", 2, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(2);
        }
    }

    ASTPtr Parser::ParseFuncDeclaration()
    {
        consume(TokensTypes::TOKEN_DEF);
        std::string var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        consume(TokensTypes::TOKEN_COLON);
        auto type = consume(current().type).type;
        consume(TokensTypes::TOKEN_LPAREN);
        std::vector<ASTPtr> args;

        while (!check(TokensTypes::TOKEN_RPAREN))
        {
            args.push_back(ParseFuncExpressions());
            while (check(TokensTypes::TOKEN_COMMA))
            {
                consume(TokensTypes::TOKEN_COMMA);
                args.push_back(ParseFuncExpressions());
            }
        }
        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET);
        // the ParseBlock will parse the [ and ] too,
        // don't need to consume the [ and ] tokens here
        auto block = ParseBlock();
        return std::make_shared<FunctionDefinitionNode>(var_name, type, args, block);
    }

    ASTPtr Parser::ParseIntVal()
    {

        ASTPtr left, right;
        TokensTypes tk;

        left = ParseNumeralExpression();
        // check if have a binary operator (like +, /, -, *) after value
        while (isBinaryOperator(current().type))
        {
            tk = consume(current().type).type;
            switch (current().type)
            {
            case TokensTypes::TOKEN_INT_32:
            case TokensTypes::TOKEN_INT_64:
            case TokensTypes::TOKEN_FLOAT_32:
            case TokensTypes::TOKEN_FLOAT_64:
                right = ParseNumeralExpression();
                while (isBinaryOperator(current().type))
                {
                    auto l = right;
                    auto tk = consume(current().type).type;
                    auto r = ParseNumeralExpression();
                    // pass the right value to left operation
                    //  right -> left value
                    return std::make_shared<BinOp>(l, tk, r);
                    break;
                }
                break;

            case TokensTypes::TOKEN_LPAREN:
            {
                TokensTypes op;
                ASTPtr l, r;
                consume(TokensTypes::TOKEN_LPAREN);
                while (!check(TokensTypes::TOKEN_RPAREN))
                {
                    l = ParseNumeralExpression();
                    while (isBinaryOperator(current().type))
                    {
                        op = consume(current().type).type;
                        r = ParseNumeralExpression();
                        break;
                    }
                    return std::make_shared<BinOp>(l, op, r);
                }
                consume(TokensTypes::TOKEN_RPAREN);
            }
            default:
                LogErrors::getInstance().addError("Expected a number or variable name after binary operator", 197, current().line, current().column);
                return nullptr; // return a nullptr to continue the error progression
            }
        }
        // by default, return que astptr according with the type
        return left;
    }

    ASTPtr Parser::ParseNumeralExpression()
    {
        ASTPtr val;
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT_32:
            val = std::make_shared<i32Node>(std::stoi(consume(TokensTypes::TOKEN_INT_32).value));
            break;
        case TokensTypes::TOKEN_INT_64:
            val = std::make_shared<i64Node>(std::stoll(consume(TokensTypes::TOKEN_INT_64).value));
            break;
        case TokensTypes::TOKEN_FLOAT_32:
            val = std::make_shared<f32Node>(std::stof(consume(TokensTypes::TOKEN_FLOAT_32).value));
            break;
        case TokensTypes::TOKEN_FLOAT_64:
            // std::cout << "Value in int: " << std::fixed << std::setprecision(current().value.size() - 1) << std::stod(current().value) << std::endl;
            val = std::make_shared<f64Node>(std::stod(consume(TokensTypes::TOKEN_FLOAT_64).value));
            break;
        }
        return val;
    }

    ASTPtr Parser::ParseIfStatement()
    {

        consume(TokensTypes::TOKEN_IF); // consume the'if'
        consume(TokensTypes::TOKEN_LPAREN);
        auto condition = ParseIfExpressions(); // conditional expression (e.g.: 1 == 2) or others
        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET); // -> consume the arrow set
        consume(TokensTypes::TOKEN_LBRACKET);
        auto ifBranch = ParseDeclarations(); // if body
        consume(TokensTypes::TOKEN_RBRACKET);
        ASTPtr butBranch = nullptr;
        ASTPtr butCondition = nullptr;

        if (check(TokensTypes::TOKEN_BUT))
        {
            consume(TokensTypes::TOKEN_BUT); // consume the 'but'
            if (check(TokensTypes::TOKEN_LPAREN))
            {
                consume(TokensTypes::TOKEN_LPAREN);
                while (!check(TokensTypes::TOKEN_RPAREN))
                {
                    // checa se há uma condição no 'but'
                    butCondition = ParseIfExpressions();
                }
                consume(TokensTypes::TOKEN_RPAREN);
                consume(TokensTypes::TOKEN_LBRACKET);
                while (!check(TokensTypes::TOKEN_RBRACKET))
                {
                    butBranch = ParseDeclarations(); // 'but' body
                }
            }
            else
            {
                consume(TokensTypes::TOKEN_LBRACKET);
                butBranch = ParseDeclarations();
            }
            consume(TokensTypes::TOKEN_RBRACKET);
        }

        return std::make_shared<IfStatement>(condition, ifBranch, butBranch, butCondition);
    }

    ASTPtr Parser::ParseIfExpressions()
    {
        auto exp_node = std::make_shared<IfExpressionNode>();
        while (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_IDENTIFIER))
            {
                exp_node->var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                while (isConditionOperator(current().type))
                {
                    exp_node->type = consume(current().type).type;
                    switch (consume(current().type).type)
                    {
                    case TokensTypes::TOKEN_INT_32:
                        exp_node->val = std::make_shared<i32Node>(std::stoi(consume(current().type).value));
                        break;
                    case TokensTypes::TOKEN_FLOAT_32:
                        exp_node->val = std::make_shared<f32Node>(std::stof(consume(current().type).value));
                        break;
                    case TokensTypes::TOKEN_FLOAT_64:
                        exp_node->val = std::make_shared<f64Node>(std::stod(consume(current().type).value));
                        break;
                    }
                }
            }
            else if (isTrueOrFalseOperator(current().type))
            {
                exp_node->type = consume(current().type).type;
            }
        }
        return exp_node;
    }

    ASTPtr Parser::ParsePrint()
    {
        auto node = std::make_shared<PrintNode>();
        consume(TokensTypes::TOKEN_PRINT);
        consume(TokensTypes::TOKEN_LPAREN);
        while (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS))
                {
                    consume(TokensTypes::TOKEN_PLUS);
                    node->val += consume(current().type).value;
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                return std::make_shared<NilNode>();
            }
            else
            {
                LogErrors::getInstance().addError("Concatenation needs a predominant value (like str, int or others types)", 3, current().line, current().column);
                continue;
            }
        }
        node->val = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
        consume(TokensTypes::TOKEN_RPAREN);
        return node;
    }

    ASTPtr Parser::ParseCinput()
    {
        consume(TokensTypes::TOKEN_CINPUT);
        consume(TokensTypes::TOKEN_LPAREN);
        if (check(TokensTypes::TOKEN_STRING_LITERAL))
        {
            auto msg = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
            return std::make_shared<CinputNode>(msg);

            if (check(TokensTypes::TOKEN_COMMA))
            {
                return std::make_shared<CinputNode>(msg, std::stoi(consume(TokensTypes::TOKEN_INT_32).value));
            }
        }
        consume(TokensTypes::TOKEN_RPAREN);
        return std::make_shared<CinputNode>();
    }

    ASTPtr Parser::ParsePrintE()
    {
        // TODO: remove the printe, print and cinput
        auto node = std::make_shared<PrintE>();
        consume(TokensTypes::TOKEN_PRINT_ERROR);
        consume(TokensTypes::TOKEN_LPAREN);
        while (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS))
                {
                    consume(TokensTypes::TOKEN_PLUS);
                    node->val += consume(current().type).value;
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                return std::make_shared<NilNode>();
            }
            else
            {
                LogErrors::getInstance().addError("Concatenation needs a predominant value to concats with other values types", 3, current().line, current().column);
                continue;
            }
        }
        consume(TokensTypes::TOKEN_RPAREN);
        return node;
    }

    ASTPtr Parser::ParsePrintNl()
    {
        auto node = std::make_shared<PrintNl>();
        consume(TokensTypes::TOKEN_PRINT_NEW_LINE);
        consume(TokensTypes::TOKEN_LPAREN);
        while (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS))
                {
                    consume(TokensTypes::TOKEN_PLUS);
                    node->val += consume(current().type).value;
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                return std::make_shared<NilNode>();
            }
        }
        consume(TokensTypes::TOKEN_RPAREN);
        return node;
    }

    ASTPtr Parser::ParseVarCall()
    {
        auto var_node = std::make_shared<VariableNode>();
        var_node->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        return var_node;
    }

    ASTPtr Parser::ParseVarDeclaration()
    {
        consume(TokensTypes::TOKEN_DEF);
        std::string name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        consume(TokensTypes::TOKEN_COLON);
        TokensTypes tk;
        switch (current().type)
        {
        case TokensTypes::TOKEN_BOOL:
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
        case TokensTypes::TOKEN_CHARSEQ:
        case TokensTypes::TOKEN_BYTES:
        case TokensTypes::TOKEN_OBJECT:
            tk = consume(current().type).type;
            break;
        default:
            // don't have support imediatelly for identifiers.. before i will put a support, not now.]
            // but is easy to set the support but a i need to create a function to parse variables names and args (if the variable is a function)
            LogErrors::getInstance().addError("Invalid type for variable definition ", 54, current().line, current().column);
            return nullptr;
        }
        // consume the ':=' to get the value
        consume(TokensTypes::TOKEN_ASSIGN);
        // parse value after assign based on the defined type on tk
        auto val = ParseExpression(tk);
        return std::make_shared<VariableDefinitionNode>(name, tk, val);
    }

    ASTPtr Parser::ParseFuncExpressions()
    {
        auto exp_node = std::make_shared<ExpressionNode>();
        exp_node->var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        consume(TokensTypes::TOKEN_COLON);
        exp_node->type = consume(current().type).type;
        return exp_node;
    }

    ASTPtr Parser::ParseExpression(TokensTypes types)
    {
        // consume the value based on the type of the variable
        // this helps the semantic analysis
        switch (types)
        {
        case TokensTypes::TOKEN_CHARSEQ:
            return std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value);
        case TokensTypes::TOKEN_PLUS:
            consume(TokensTypes::TOKEN_PLUS);
            return ParsePositiveVals(consume(current().type).type);
        case TokensTypes::TOKEN_MINUS:
            consume(TokensTypes::TOKEN_MINUS);
            return ParseNegativeVals(consume(current().type).type);
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            return ParseIntVal();
        case TokensTypes::TOKEN_BYTES:
            return ParseByteVal();
        case TokensTypes::TOKEN_BOOL:
            return ParseLoopCondition();
        case TokensTypes::TOKEN_OBJECT:
            switch (current().type)
            {
            case TokensTypes::TOKEN_INT_32:
            case TokensTypes::TOKEN_INT_64:
            case TokensTypes::TOKEN_FLOAT_32:
            case TokensTypes::TOKEN_FLOAT_64:
                return std::make_shared<ObjectNode>(ParseNumeralExpression());
            case TokensTypes::TOKEN_TRUE:
            {
                consume(TokensTypes::TOKEN_TRUE);
                auto ptr = std::make_shared<TrueOrFalseNode>(true);
                return std::make_shared<ObjectNode>(ptr);
            }
            case TokensTypes::TOKEN_FALSE:
            {
                consume(TokensTypes::TOKEN_FALSE);
                auto ptr = std::make_shared<TrueOrFalseNode>(false);
                return std::make_shared<ObjectNode>(ptr);
            }
            case TokensTypes::TOKEN_STRING_LITERAL:
            {
                auto ptr = std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value);
                return std::make_shared<ObjectNode>(ptr);
            }
            case TokensTypes::TOKEN_IDENTIFIER:
            {
                ASTPtr ptr;
                if (check(TokensTypes::TOKEN_LPAREN))
                {
                    auto id = std::make_shared<IdentifierNode>();
                    id->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    consume(TokensTypes::TOKEN_LPAREN);
                    while (!check(TokensTypes::TOKEN_RPAREN))
                    {
                        switch (current().type)
                        {
                        case TokensTypes::TOKEN_IDENTIFIER:
                            consume(TokensTypes::TOKEN_IDENTIFIER);
                            std::cout << "Identifier podi n man" << std::endl;
                            break;
                        case TokensTypes::TOKEN_INT_32:
                        case TokensTypes::TOKEN_INT_64:
                        case TokensTypes::TOKEN_FLOAT_32:
                        case TokensTypes::TOKEN_FLOAT_64:
                            id->args.push_back(ParseNumeralExpression());
                            break;
                        case TokensTypes::TOKEN_TRUE:
                        case TokensTypes::TOKEN_FALSE:
                            id->args.push_back(ParseLoopCond());
                            break;
                        }
                        consume(TokensTypes::TOKEN_RPAREN);
                        ptr = id;
                    }
                }
                else
                {
                    auto id = std::make_shared<VariableNode>();
                    id->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    ptr = id;
                }
                return std::make_shared<ObjectNode>(ptr);
            }
            default:
                LogErrors::getInstance().addError("Invalid variable value for 'obj' type", 95, current().line, current().column);
                return nullptr;
            }
        default:
            LogErrors::getInstance().addError("Invalid variable value type", 97, current().line, current().column);
            return nullptr;
            // throw Excepts::CompilationException("Invalid Variable Value Type");
        }
    }

    ASTPtr Parser::ParsePositiveVals(TokensTypes curr)
    {
        switch (curr)
        {
        case TokensTypes::TOKEN_INT_32:
            try
            {
                return std::make_shared<i32Node>(+std::stoi(consume(TokensTypes::TOKEN_INT_32).value));
            }
            catch (std::out_of_range e)
            {
                std::cerr << "[Error]: Current value out of range at line " << current().line << " column " << current().column << std::endl;
                return nullptr;
                // throw std::out_of_range("Index out of range");
            }

        case TokensTypes::TOKEN_FLOAT_64:
            return std::make_shared<f64Node>(+std::stod(consume(current().type).value));
        case TokensTypes::TOKEN_FLOAT_32:
            return std::make_shared<f32Node>(+std::stof(consume(current().type).value));
        default:
            throw Excepts::CompilationException("Invalid positive type");
        }
    }

    ASTPtr Parser::ParseNegativeVals(TokensTypes tks)
    {
        switch (tks)
        {
        case TokensTypes::TOKEN_INT_32:
            try
            {
                return std::make_shared<i32Node>(-std::stoi(consume(TokensTypes::TOKEN_INT_32).value));
            }
            catch (std::out_of_range e)
            {
                std::cerr << "[Error]: Current value out of range at line " << current().line << " column " << current().column << std::endl;
                // throw std::out_of_range("Index out of range");
                return nullptr;
            }

        case TokensTypes::TOKEN_FLOAT_64:
            return std::make_shared<f64Node>(-std::stod(consume(current().type).value));
        case TokensTypes::TOKEN_FLOAT_32:
            return std::make_shared<f32Node>(-std::stof(consume(current().type).value));
        default:
            throw Excepts::CompilationException("Invalid negative type");
        }
    }

    ASTPtr Parser::ParseLoopCondition()
    {
        return consume(current().type).type == TokensTypes::TOKEN_TRUE ? (std::make_shared<TrueOrFalseNode>(true)) : (std::make_shared<TrueOrFalseNode>(false));
    }

    ASTPtr Parser::ParseLoopExpression()
    {
        consume(TokensTypes::TOKEN_LOOP);
        consume(TokensTypes::TOKEN_LPAREN);

        std::string var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        // consome o : para em seguida consumir o tipo da variavel
        consume(TokensTypes::TOKEN_COLON);
        // switch para verificação de tipos da variavel
        TokensTypes type;
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            type = consume(current().type).type;
            break;
        default:
            LogErrors::getInstance().addError("Loop expression only accepts number types (int(32 or 64), float(32/64)) and array lists (foreach)", 23, current().line, current().column);
            return nullptr;
        }
        consume(TokensTypes::TOKEN_IN);
        ASTPtr val;
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            val = ParseNumeralExpression();
            break;
        default:
            LogErrors::getInstance().addError("Invalid type for loop expression", 23, current().line, current().column);
            return nullptr;
        }
        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET);
        auto block = ParseBlock();
        return std::make_shared<LoopNode>(var_name, type, val, block);
    }

    ASTPtr Parser::ParseLoopCond()
    {
        consume(TokensTypes::TOKEN_LOOP);
        consume(TokensTypes::TOKEN_LPAREN);
        auto node = std::make_shared<LoopConditionNode>();
        /// ParseLoopExpression will be check if have a varaible definition and his type or condition
        switch (current().type)
        {
        case TokensTypes::TOKEN_FALSE:
        case TokensTypes::TOKEN_TRUE:
            node->condition = ParseLoopCondition();
            break;
        case TokensTypes::TOKEN_IDENTIFIER:
            node->condition = ParseVarCall();
            break;
        }

        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET);

        node->body = ParseBlock();

        return node;
    }

    ASTPtr Parser::ParseByteVal()
    {
        long long lit_val;
        unsigned char val;
        if (check(TokensTypes::TOKEN_INT_32))
        {
            lit_val = std::stoll(consume(current().type).value);
        }

        if (lit_val < 0 || lit_val > 255)
        {
            LogErrors::getInstance().addWarning("Value " + std::to_string(lit_val) + " is out of range for byte type. It will be truncated to: " + std::to_string(static_cast<unsigned char>(lit_val)), 5, current().line, current().column);
            val = static_cast<unsigned char>(lit_val);
        }
        else
        {
            val = static_cast<unsigned char>(lit_val);
        }
        if (check(TokensTypes::TOKEN_NIL))
        // nil cannot be assigned with number values (int32, int64, float32, flat64 and others), throw a error pls
        {
            // this error wil be repeated with ParseIntValues
            LogErrors::getInstance().addError("'nil' keyword cannot be assigned with number values", 89, current().line, current().column);
        }
        else
        {
            consume(TokensTypes::TOKEN_INT_32);
        }
        return std::make_shared<ByteNode>(val);
    }

    ASTPtr Parser::ParseBlock()
    {
        auto block = std::make_shared<BlockNode>();
        consume(TokensTypes::TOKEN_LBRACKET); // '['

        while (!check(TokensTypes::TOKEN_RBRACKET))
        {
            block->statements.push_back(ParseDeclarations());
        }
        /*if (!lookAhead(TokensTypes::TOKEN_RBRACKET))
        {
            LogErrors::getInstance().addError("Function scope not closed yet", 57, current().line, current().column);
        }*/
        consume(TokensTypes::TOKEN_RBRACKET); // ']'
        return block;
    }
}