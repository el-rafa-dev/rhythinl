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
        // If position is out of bounds, return EOF (End Of File) token.
        // This ensures the parser doesn't crash on out-of-bounds access.
        return tokens[tokens.size() - 1];
    }

    // Peeks at a token relative to the current position without consuming it.
    // Useful for lookahead decisions.
    Tokens Parser::peek(int offset)
    {
        int peek_pos = position + offset;
        if (peek_pos >= 0 && peek_pos < tokens.size())
        {
            return tokens[peek_pos];
        }
        // Return EOF if peeking beyond the file
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
            LogErrors::getInstance().printAll(); // Added to exit on error immediately
            exit(4); // Exit with a different code
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

    // This lookAhead function was problematic. A simpler `peek` is introduced for reliability.
    // Keeping it here for reference, but not used in the new logic.
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
            // Resolving loop ambiguity:
            // Loop can be `loop (condition) ->` or `loop (var:type in value) ->`.
            // We need to look ahead after `loop (` to differentiate.
            // Consume LOOP and LPAREN first, then peek to decide.
            int start_pos = position; // Save position to restore if needed
            consume(TokensTypes::TOKEN_LOOP);
            consume(TokensTypes::TOKEN_LPAREN);

            // Check if it's a loop expression (e.g., `var:type in value`)
            // This means we expect an IDENTIFIER, then a COLON, then a TYPE, then 'in'.
            if (check(TokensTypes::TOKEN_IDENTIFIER) &&
                peek(1).type == TokensTypes::TOKEN_COLON &&
                (peek(2).type == TokensTypes::TOKEN_INT_32 || peek(2).type == TokensTypes::TOKEN_INT_64 ||
                 peek(2).type == TokensTypes::TOKEN_FLOAT_32 || peek(2).type == TokensTypes::TOKEN_FLOAT_64) &&
                peek(3).type == TokensTypes::TOKEN_IN)
            {
                position = start_pos; // Restore position, ParseLoopExpression will consume from TOKEN_LOOP
                return ParseLoopExpression();
            }
            else
            {
                position = start_pos; // Restore position, ParseLoopCond will consume from TOKEN_LOOP
                return ParseLoopCond();
            }
        }

        case TokensTypes::TOKEN_PRINT:
            return ParsePrint();
        case TokensTypes::TOKEN_PRINT_ERROR:
            return ParsePrintE();
        case TokensTypes::TOKEN_PRINT_NEW_LINE:
            return ParsePrintNl();
        case TokensTypes::TOKEN_STRING_LITERAL: // This case just consumes and returns nullptr, which might not be intended for a top-level declaration.
            consume(TokensTypes::TOKEN_STRING_LITERAL);
            return nullptr; // Consider what this should actually do
        case TokensTypes::TOKEN_DEF:
        {
            // stores the current position on another int
            int pos = position;
            // look ahead if have the assign (=) token
            // if not, is check if have the left paren ( token
            // the ( token defines thats the variable is a function regardless of type
            while (!check(TokensTypes::TOKEN_ASSIGN) && !check(TokensTypes::TOKEN_LPAREN)) // Added check for LPAREN in loop condition
            {
                if (current().type == TokensTypes::TOKEN_EOF) { // Avoid infinite loop on unexpected token
                     LogErrors::getInstance().addError("Unexpected EOF while parsing declaration. Missing '=' or '('?", 1, current().line, current().column);
                     LogErrors::getInstance().printAll();
                     exit(1);
                }
                consume(current().type);
            }

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
            else
            {
                // Should not reach here if the while loop is correct, but as a safeguard:
                LogErrors::getInstance().addError("Expected '=' or '(' after 'def IDENTIFIER : TYPE' in declaration", 5, current().line, current().column);
                LogErrors::getInstance().printAll();
                exit(5);
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

    // --- Arithmetic Expression Parsing (Resolved Ambiguity & Precedence) ---

    // Parses primary expressions: numbers, identifiers, or expressions in parentheses.
    ASTPtr Parser::ParsePrimaryExpression()
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
            val = std::make_shared<f64Node>(std::stod(consume(TokensTypes::TOKEN_FLOAT_64).value));
            break;
        case TokensTypes::TOKEN_LPAREN: // Handle parenthesized expressions (e.g., (1 + 2) * 3)
            consume(TokensTypes::TOKEN_LPAREN);
            val = ParseAdditiveExpression(); // Recursively parse the expression inside parentheses
            consume(TokensTypes::TOKEN_RPAREN);
            break;
        case TokensTypes::TOKEN_IDENTIFIER: // Handle variable calls
            val = ParseVarCall();
            break;
        case TokensTypes::TOKEN_MINUS: // Handle unary minus
            consume(TokensTypes::TOKEN_MINUS);
            val = std::make_shared<UnaryOp>(TokensTypes::TOKEN_MINUS, ParsePrimaryExpression());
            break;
        case TokensTypes::TOKEN_PLUS: // Handle unary plus (optional)
            consume(TokensTypes::TOKEN_PLUS);
            val = std::make_shared<UnaryOp>(TokensTypes::TOKEN_PLUS, ParsePrimaryExpression());
            break;
        default:
            LogErrors::getInstance().addError("Expected a number, identifier, or '(' for expression", 197, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(197);
        }
        return val;
    }

    // Parses multiplicative expressions (multiplication, division)
    // Uses left-to-right associativity and calls ParsePrimaryExpression for operands.
    ASTPtr Parser::ParseMultiplicativeExpression()
    {
        ASTPtr left = ParsePrimaryExpression();
        while (check(TokensTypes::TOKEN_PLUS) || check(TokensTypes::TOKEN_DIVIDE) || check(TokensTypes::TOKEN_MODULO))
        {
            TokensTypes op = consume(current().type).type;
            ASTPtr right = ParsePrimaryExpression();
            left = std::make_shared<BinOp>(left, op, right);
        }
        return left;
    }

    // Parses additive expressions (addition, subtraction)
    // Uses left-to-right associativity and calls ParseMultiplicativeExpression for operands,
    // ensuring correct operator precedence.
    ASTPtr Parser::ParseAdditiveExpression()
    {
        ASTPtr left = ParseMultiplicativeExpression();
        while (check(TokensTypes::TOKEN_PLUS) || check(TokensTypes::TOKEN_MINUS))
        {
            TokensTypes op = consume(current().type).type;
            ASTPtr right = ParseMultiplicativeExpression();
            left = std::make_shared<BinOp>(left, op, right);
        }
        return left;
    }

    // Renamed from ParseIntVal to reflect its new role as the entry point for
    // numerical and arithmetic expression parsing, handling precedence correctly.
    // This function will now correctly parse chained operations and operator precedence.
    ASTPtr Parser::ParseIntVal()
    {
        return ParseAdditiveExpression();
    }
    // --- End Arithmetic Expression Parsing ---


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
        default: // Added default case to catch non-numeral tokens
            LogErrors::getInstance().addError("Expected a numeral literal (int32, float32, etc.)", 198, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(198);
        }
        return val;
    }

    ASTPtr Parser::ParseIfStatement()
    {

        consume(TokensTypes::TOKEN_IF);      // consume the'if'
        consume(TokensTypes::TOKEN_LPAREN);
        auto condition = ParseIfExpressions(); // conditional expression (e.g.: 1 == 2) or others
        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET); // -> consume the arrow set
        //consume(TokensTypes::TOKEN_LBRACKET); // dont need it here, the ParseBlock already consumes the [... works smart, not hard hehe
        auto ifBranch = ParseBlock(); // if body (ParseBlock handles brackets)
        //consume(TokensTypes::TOKEN_RBRACKET); // Removed, as ParseBlock consumes ']'
        ASTPtr butBranch = nullptr;
        ASTPtr butCondition = nullptr;

        if (check(TokensTypes::TOKEN_BUT))
        {
            consume(TokensTypes::TOKEN_BUT); // consume the 'but'
            if (check(TokensTypes::TOKEN_LPAREN))
            {
                consume(TokensTypes::TOKEN_LPAREN);
                // While loop for parsing multiple conditions in 'but' might be tricky, assuming single condition for now
                while (!check(TokensTypes::TOKEN_RPAREN)) // Check if not immediately closing
                {
                    butCondition = ParseIfExpressions();
                }
                consume(TokensTypes::TOKEN_RPAREN);
            }
            consume(TokensTypes::TOKEN_ARROW_SET); // Added to match 'if' structure for 'but'
            consume(TokensTypes::TOKEN_LBRACKET);
            butBranch = ParseBlock(); // 'but' body (ParseBlock handles brackets)
            // consume(TokensTypes::TOKEN_RBRACKET); // Removed, as ParseBlock consumes ']'
        }

        return std::make_shared<IfStatement>(condition, ifBranch, butBranch, butCondition);
    }

    ASTPtr Parser::ParseIfExpressions()
    {
        auto exp_node = std::make_shared<IfExpressionNode>();
        // Assuming ParseIfExpressions parses a single comparison or boolean literal for now.
        // For more complex boolean logic (AND, OR), this function would need to be expanded.
        if (check(TokensTypes::TOKEN_IDENTIFIER))
        {
            exp_node->var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
            if (isConditionOperator(current().type)) // check if current token is a condition operator
            {
                exp_node->type = consume(current().type).type; // Consume the operator
                switch (current().type) // Check the type of the value after the operator
                {
                case TokensTypes::TOKEN_INT_32:
                    exp_node->val = std::make_shared<i32Node>(std::stoi(consume(TokensTypes::TOKEN_INT_32).value));
                    break;
                case TokensTypes::TOKEN_INT_64: // Added INT_64 support
                    exp_node->val = std::make_shared<i64Node>(std::stoll(consume(TokensTypes::TOKEN_INT_64).value));
                    break;
                case TokensTypes::TOKEN_FLOAT_32:
                    exp_node->val = std::make_shared<f32Node>(std::stof(consume(TokensTypes::TOKEN_FLOAT_32).value));
                    break;
                case TokensTypes::TOKEN_FLOAT_64:
                    exp_node->val = std::make_shared<f64Node>(std::stod(consume(TokensTypes::TOKEN_FLOAT_64).value));
                    break;
                case TokensTypes::TOKEN_STRING_LITERAL: // Added string literal support for comparison
                    exp_node->val = std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value);
                    break;
                case TokensTypes::TOKEN_IDENTIFIER: // Added identifier support for comparison (variable vs variable)
                {
                    auto var = std::make_shared<VariableNode>();
                    var->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    exp_node->val = var;
                    break;
                }
                default:
                    LogErrors::getInstance().addError("Expected a value or identifier after conditional operator", 200, current().line, current().column);
                    LogErrors::getInstance().printAll();
                    exit(200);
                }
            } else {
                 LogErrors::getInstance().addError("Expected a conditional operator (==, !=, >, etc.) after identifier in if expression", 201, current().line, current().column);
                 LogErrors::getInstance().printAll();
                 exit(201);
            }
        }
        else if (isTrueOrFalseOperator(current().type))
        {
            exp_node->type = consume(current().type).type; // Consumes TRUE or FALSE
            // If it's a direct boolean literal, val should likely be a TrueOrFalseNode directly.
            // Current setup uses 'type' for the boolean operator (TRUE/FALSE) which is fine for direct literals.
            exp_node->val = std::make_shared<TrueOrFalseNode>(exp_node->type == TokensTypes::TOKEN_TRUE);
        }
        else
        {
            LogErrors::getInstance().addError("Invalid expression in 'if' condition. Expected identifier or boolean literal.", 202, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(202);
        }
        // Original loop 'while (!check(TokensTypes::TOKEN_RPAREN))' here was problematic if not properly handled
        // Assuming ParseIfExpressions parses a single condition like `id == value` or `true/false`
        // More complex conditions (e.g., `a == b && c < d`) would require a separate boolean expression parser.
        return exp_node;
    }

    ASTPtr Parser::ParsePrint()
    {
        auto node = std::make_shared<PrintNode>();
        consume(TokensTypes::TOKEN_PRINT);
        consume(TokensTypes::TOKEN_LPAREN);
        // Changed loop condition to ensure at least one value is parsed before closing ')'
        if (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS))
                {
                    consume(TokensTypes::TOKEN_PLUS);
                    // Ensure the next token is also a string literal for concatenation
                    if (check(TokensTypes::TOKEN_STRING_LITERAL))
                    {
                        node->val += consume(current().type).value;
                    }
                    else if (check(TokensTypes::TOKEN_IDENTIFIER)) // Allows string concatenation with variable
                    {
                        // TODO: semantic analysis will need to verify if the identifier is a charseq
                        node->val += consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    }
                    else
                    {
                        LogErrors::getInstance().addError("Only string literals or identifiers can be concatenated with '+' in print for now.", 3, current().line, current().column);
                        LogErrors::getInstance().printAll();
                        exit(3);
                    }
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                consume(TokensTypes::TOKEN_NIL); // Consume NIL
                return std::make_shared<NilNode>(); // Return NilNode if print(nil)
            }
            else if (check(TokensTypes::TOKEN_IDENTIFIER)) // Allow printing identifiers directly
            {
                node->val = consume(TokensTypes::TOKEN_IDENTIFIER).value;
            }
            else if (current().type == TokensTypes::TOKEN_INT_32 || current().type == TokensTypes::TOKEN_INT_64 ||
                     current().type == TokensTypes::TOKEN_FLOAT_32 || current().type == TokensTypes::TOKEN_FLOAT_64) // Allow printing numbers directly
            {
                node->val = current().value; // Store the string representation of the number
                consume(current().type); // Consume the number token
            }
            else
            {
                LogErrors::getInstance().addError("Print statement supports string literals, identifiers, numbers, or 'nil'", 3, current().line, current().column);
                LogErrors::getInstance().printAll();
                exit(3);
            }
        }
        else
        {
             // Handle empty print()
             LogErrors::getInstance().addWarning("Empty print statement. Consider printing a newline with printnl()", 300, current().line, current().column);
        }
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
            // The comma logic was inside a return which made it unreachable.
            // Re-structured to allow message and optional integer.
            if (check(TokensTypes::TOKEN_COMMA))
            {
                consume(TokensTypes::TOKEN_COMMA);
                if (check(TokensTypes::TOKEN_INT_32))
                {
                    return std::make_shared<CinputNode>(msg, std::stoi(consume(TokensTypes::TOKEN_INT_32).value));
                }
                else
                {
                    LogErrors::getInstance().addError("Expected an integer after comma in cinput()", 301, current().line, current().column);
                    LogErrors::getInstance().printAll();
                    exit(301);
                }
            }
            else
            {
                // No comma, so only message provided
                consume(TokensTypes::TOKEN_RPAREN); // Ensure RPAREN is consumed after message
                return std::make_shared<CinputNode>(msg);
            }
        }
        else
        {
            // If no string literal, assume cinput() with no arguments
            LogErrors::getInstance().addWarning("cinput() called without a message. Consider adding a prompt string.", 302, current().line, current().column);
        }

        consume(TokensTypes::TOKEN_RPAREN);
        return std::make_shared<CinputNode>(); // Default cinput() if no specific arguments parsed
    }

    ASTPtr Parser::ParsePrintE()
    {
        // TODO: remove the printe, print and cinput - This comment is from original code.
        auto node = std::make_shared<PrintE>();
        consume(TokensTypes::TOKEN_PRINT_ERROR);
        consume(TokensTypes::TOKEN_LPAREN);
        // Changed loop condition to ensure at least one value is parsed before closing ')'
        if (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS))
                {
                    consume(TokensTypes::TOKEN_PLUS);
                     if (check(TokensTypes::TOKEN_STRING_LITERAL))
                    {
                        node->val += consume(current().type).value;
                    }
                    else if (check(TokensTypes::TOKEN_IDENTIFIER))
                    {
                        node->val += consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    }
                    else
                    {
                        LogErrors::getInstance().addError("Only string literals or identifiers can be concatenated with '+' in print_error for now.", 3, current().line, current().column);
                        LogErrors::getInstance().printAll();
                        exit(3);
                    }
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                consume(TokensTypes::TOKEN_NIL);
                return std::make_shared<NilNode>();
            }
            else if (check(TokensTypes::TOKEN_IDENTIFIER))
            {
                node->val = consume(TokensTypes::TOKEN_IDENTIFIER).value;
            }
            else if (current().type == TokensTypes::TOKEN_INT_32 || current().type == TokensTypes::TOKEN_INT_64 ||
                     current().type == TokensTypes::TOKEN_FLOAT_32 || current().type == TokensTypes::TOKEN_FLOAT_64)
            {
                node->val = current().value;
                consume(current().type);
            }
            else
            {
                LogErrors::getInstance().addError("Print_error statement supports string literals, identifiers, numbers, or 'nil'", 3, current().line, current().column);
                LogErrors::getInstance().printAll();
                exit(3);
            }
        }
        else
        {
            LogErrors::getInstance().addWarning("Empty print_error statement.", 303, current().line, current().column);
        }
        consume(TokensTypes::TOKEN_RPAREN);
        return node;
    }

    ASTPtr Parser::ParsePrintNl()
    {
        auto node = std::make_shared<PrintNl>();
        consume(TokensTypes::TOKEN_PRINT_NEW_LINE);
        consume(TokensTypes::TOKEN_LPAREN);
        // Changed loop condition to ensure at least one value is parsed before closing ')'
        if (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS))
                {
                    consume(TokensTypes::TOKEN_PLUS);
                    if (check(TokensTypes::TOKEN_STRING_LITERAL))
                    {
                        node->val += consume(current().type).value;
                    }
                    else if (check(TokensTypes::TOKEN_IDENTIFIER))
                    {
                        node->val += consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    }
                    else
                    {
                        LogErrors::getInstance().addError("Only string literals or identifiers can be concatenated with '+' in print_nl for now.", 3, current().line, current().column);
                        LogErrors::getInstance().printAll();
                        exit(3);
                    }
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                consume(TokensTypes::TOKEN_NIL);
                return std::make_shared<NilNode>();
            }
            else if (check(TokensTypes::TOKEN_IDENTIFIER))
            {
                node->val = consume(TokensTypes::TOKEN_IDENTIFIER).value;
            }
            else if (current().type == TokensTypes::TOKEN_INT_32 || current().type == TokensTypes::TOKEN_INT_64 ||
                     current().type == TokensTypes::TOKEN_FLOAT_32 || current().type == TokensTypes::TOKEN_FLOAT_64)
            {
                node->val = current().value;
                consume(current().type);
            }
        }
        else
        {
            LogErrors::getInstance().addWarning("Empty print_nl statement.", 304, current().line, current().column);
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
            LogErrors::getInstance().printAll();
            exit(54);
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
        case TokensTypes::TOKEN_PLUS: // This case handles unary plus at the start of an expression
            consume(TokensTypes::TOKEN_PLUS);
            return ParsePositiveVals(current().type); // ParsePositiveVals now expects the type to consume
        case TokensTypes::TOKEN_MINUS: // This case handles unary minus at the start of an expression
            consume(TokensTypes::TOKEN_MINUS);
            return ParseNegativeVals(current().type); // ParseNegativeVals now expects the type to consume
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            return ParseAdditiveExpression(); // Now calls the top-level arithmetic expression parser
        case TokensTypes::TOKEN_BYTES:
            return ParseByteVal();
        case TokensTypes::TOKEN_BOOL:
            return ParseLoopCondition(); // ParseLoopCondition returns a boolean node (true/false literal)
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
                if (peek(1).type == TokensTypes::TOKEN_LPAREN) // Check if it's a function call
                {
                    auto id = std::make_shared<IdentifierNode>(); // Assuming IdentifierNode for function calls with args
                    id->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    consume(TokensTypes::TOKEN_LPAREN);
                    while (!check(TokensTypes::TOKEN_RPAREN))
                    {
                        // Parse arguments for function call. Needs to handle various types.
                        // This part was incomplete and needs further robust parsing for arguments.
                        switch (current().type)
                        {
                        case TokensTypes::TOKEN_IDENTIFIER:
                        {
                            // If it's just an identifier (variable passed as arg)
                            auto var = std::make_shared<VariableNode>();
                            var->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                            id->args.push_back(var);
                            break;
                        }
                        case TokensTypes::TOKEN_INT_32:
                        case TokensTypes::TOKEN_INT_64:
                        case TokensTypes::TOKEN_FLOAT_32:
                        case TokensTypes::TOKEN_FLOAT_64:
                            id->args.push_back(ParseNumeralExpression());
                            break;
                        case TokensTypes::TOKEN_TRUE:
                        case TokensTypes::TOKEN_FALSE:
                            id->args.push_back(ParseLoopCondition()); // TrueOrFalseNode
                            break;
                        case TokensTypes::TOKEN_STRING_LITERAL: // Allow string literals as arguments
                             id->args.push_back(std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value));
                             break;
                        default:
                            LogErrors::getInstance().addError("Invalid argument type in function call", 96, current().line, current().column);
                            LogErrors::getInstance().printAll();
                            exit(96);
                        }
                        // Handle multiple arguments separated by commas (if your language supports it)
                        if (check(TokensTypes::TOKEN_COMMA)) {
                            consume(TokensTypes::TOKEN_COMMA);
                        } else if (!check(TokensTypes::TOKEN_RPAREN)) { // If not comma and not end of args, then error
                             LogErrors::getInstance().addError("Expected comma or ')' after function argument", 97, current().line, current().column);
                             LogErrors::getInstance().printAll();
                             exit(97);
                        }
                    }
                    consume(TokensTypes::TOKEN_RPAREN);
                    ptr = id;
                }
                else
                {
                    // It's a variable call
                    auto id = std::make_shared<VariableNode>();
                    id->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    ptr = id;
                }
                return std::make_shared<ObjectNode>(ptr);
            }
            default:
                LogErrors::getInstance().addError("Invalid variable value for 'obj' type", 95, current().line, current().column);
                LogErrors::getInstance().printAll();
                exit(95);
            }
        default:
            LogErrors::getInstance().addError("Invalid variable value type", 97, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(97);
            // throw Excepts::CompilationException("Invalid Variable Value Type");
        }
    }

    // needs a correction for perfectly work
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
                LogErrors::getInstance().addError("Integer value out of range", 100, current().line, current().column);
                LogErrors::getInstance().printAll();
                exit(100);
            }

        case TokensTypes::TOKEN_FLOAT_64:
            return std::make_shared<f64Node>(+std::stod(consume(TokensTypes::TOKEN_FLOAT_64).value));
        case TokensTypes::TOKEN_FLOAT_32:
            return std::make_shared<f32Node>(+std::stof(consume(TokensTypes::TOKEN_FLOAT_32).value));
        default:
            LogErrors::getInstance().addError("Invalid type for positive value.", 101, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(101);
        }
    }

    //needs a correction too to work perfectly
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
                LogErrors::getInstance().addError("Integer value out of range for negative value.", 102, current().line, current().column);
                LogErrors::getInstance().printAll();
                exit(102);
            }

        case TokensTypes::TOKEN_FLOAT_64:
            return std::make_shared<f64Node>(-std::stod(consume(TokensTypes::TOKEN_FLOAT_64).value));
        case TokensTypes::TOKEN_FLOAT_32:
            return std::make_shared<f32Node>(-std::stof(consume(TokensTypes::TOKEN_FLOAT_32).value));
        default:
            LogErrors::getInstance().addError("Invalid type for negative value.", 103, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(103);
        }
    }

    ASTPtr Parser::ParseLoopCondition()
    {
        // This function now just parses true/false literals, suitable for boolean expressions.
        if (current().type == TokensTypes::TOKEN_TRUE) {
            consume(TokensTypes::TOKEN_TRUE);
            return std::make_shared<TrueOrFalseNode>(true);
        } else if (current().type == TokensTypes::TOKEN_FALSE) {
            consume(TokensTypes::TOKEN_FALSE);
            return std::make_shared<TrueOrFalseNode>(false);
        } else {
            LogErrors::getInstance().addError("Expected 'true' or 'false' for loop condition literal.", 203, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(203);
        }
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
            LogErrors::getInstance().addError("Loop expression only accepts number types (int(32 or 64), float(32 or 64)) and array lists (foreach)", 23, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(23);
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
        case TokensTypes::TOKEN_IDENTIFIER: // Allows loop over a variable (e.g., an array)
            val = ParseVarCall();
            break;
        default:
            LogErrors::getInstance().addError("Invalid type for 'in' value in loop expression. Expected numeral or a identifier.", 23, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(23);
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
            node->condition = ParseLoopCondition(); // Parses boolean literal (true/false)
            break;
        case TokensTypes::TOKEN_IDENTIFIER:
            // This case might need to parse a full boolean expression like `var == 5`
            // For now, it parses a simple variable reference. If `loop (myVar) ->` is the intent for a boolean var.
            // If `loop (myVar == 5) ->` is needed, then ParseIfExpressions or a dedicated boolean expression parser is needed here.
            node->condition = ParseVarCall();
            break;
        // Added support for direct arithmetic/comparison expressions as loop conditions (e.g., loop (10 > x) ->)
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
             // If a number starts the expression, try to parse it as an arithmetic/comparison expression
             node->condition = ParseIfExpressions(); // Reusing ParseIfExpressions might work if it supports direct comparisons without an initial IDENTIFIER
             break;
        default:
            LogErrors::getInstance().addError("Invalid token for loop condition. Expected boolean literal, identifier, or expression.", 204, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(204);
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
        // Changed to check for any numeric type, then consume it.
        if (current().type == TokensTypes::TOKEN_INT_32 || current().type == TokensTypes::TOKEN_INT_64 ||
            current().type == TokensTypes::TOKEN_FLOAT_32 || current().type == TokensTypes::TOKEN_FLOAT_64)
        {
            // Assuming we only take the integer part for byte conversion
            lit_val = std::stoll(consume(current().type).value);
        } else {
            LogErrors::getInstance().addError("Expected a number literal for byte type", 88, current().line, current().column);
            LogErrors::getInstance().printAll();
            exit(88);
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

        // The check for TOKEN_NIL and error was problematic as nil cannot be a number.
        // If a nil is encountered here, it's an error because a number was expected for byte.
        // Removed the check for TOKEN_NIL from here as it implies an incorrect flow for byte values.
        // It's already handled by expecting a number type above.

        return std::make_shared<ByteNode>(val);
    }

    ASTPtr Parser::ParseBlock()
    {
        auto block = std::make_shared<BlockNode>();
        consume(TokensTypes::TOKEN_LBRACKET); // '['

        while (!check(TokensTypes::TOKEN_RBRACKET))
        {
            if (current().type == TokensTypes::TOKEN_EOF) {
                 LogErrors::getInstance().addError("Unclosed block. Expected ']' but reached end of file.", 57, current().line, current().column);
                 LogErrors::getInstance().printAll();
                 exit(57);
            }
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