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
        // It's crucial for error recovery to always return a valid token type,
        // even if it's an EOF when past the end.
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

        if (current().type == TokensTypes::TOKEN_EOF)
        {
            // If EOF is reached unexpectedly, add an error but allow parsing to continue
            // (e.g., if it's the end of a block that should have been closed).
            // The main Parse() loop will eventually stop on EOF.
            LogErrors::getInstance().addError("Expected a statement but reached the end of file and left early.... Are you forget anything?", 1, current().line, current().column);
            // Do NOT exit here to allow error progression.
            return Tokens{TokensTypes::TOKEN_EOF, "", current().line, current().column}; // Return EOF token
        }

        if (current().type != tk)
        {
            // If the current type doesn't match the expected token, add an error.
            // Do NOT exit here to allow error progression.
            LogErrors::getInstance().addError("Expected " + Tokens::tokenTypeToString(tk) + " token but got: " + Tokens::tokenTypeToString(current().type), 4, current().line, current().column);
            // Try to advance the parser to potentially recover from the error.
            // This is a simple recovery strategy; more advanced parsers might skip tokens.
            position++;
            return tokens[position - 1]; // Return the consumed (but incorrect) token
        }

        position++;
        return tokens[position - 1];
    }

    std::vector<ASTPtr> Parser::Parse()
    {

        std::vector<ASTPtr> node;
        while (current().type != TokensTypes::TOKEN_EOF)
        {
            ASTPtr declaration = ParseDeclarations();
            if (declaration) // Only add if parsing was successful (not nullptr due to error)
            {
                node.push_back(declaration);
            }
            else
            {
                // Error occurred in ParseDeclarations, try to recover and continue
                // Simple recovery: if it's not EOF, advance past the current token
                // to avoid infinite loops on unhandled tokens.
                if (current().type != TokensTypes::TOKEN_EOF)
                {
                    position++;
                }
                else
                {
                    // If EOF is reached, break the loop
                    break;
                }
            }
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
    /*
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
    */

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
            // Temporarily consume TOKEN_LOOP and TOKEN_LPAREN to inspect what follows.
            // This consume operation might add an error if the token is not as expected,
            // but the error is logged and parsing continues.
            Tokens loop_token = consume(TokensTypes::TOKEN_LOOP);
            if (loop_token.type != TokensTypes::TOKEN_LOOP)
            {                         // Check if consume failed
                position = start_pos; // Restore position if consume failed for initial token
                return nullptr;
            }
            Tokens lparen_token = consume(TokensTypes::TOKEN_LPAREN);
            if (lparen_token.type != TokensTypes::TOKEN_LPAREN)
            {                         // Check if consume failed
                position = start_pos; // Restore position if consume failed
                return nullptr;
            }

            // Check if it's a loop expression (e.g., `var:type in value`)
            // This means we expect an IDENTIFIER, then a COLON, then a TYPE, then 'in'.
            // Ensure proper token types are checked for types (INT_32, INT_64, FLOAT_32, FLOAT_64).
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
            // error only for test
            // LogErrors::getInstance().addError("Unexpected string literal at top-level. Expected a statement.", 2, current().line, current().column);
            return nullptr; // Return nullptr for error progression
        case TokensTypes::TOKEN_DEF:
        {
            // stores the current position on another int
            int pos = position;
            // Advance until '=' or '(' is found, or EOF, to determine if it's a variable or function declaration.
            // This consume loop should not add errors for simply advancing. Errors are added by final checks.
            while (!check(TokensTypes::TOKEN_ASSIGN) && !check(TokensTypes::TOKEN_LPAREN) && current().type != TokensTypes::TOKEN_EOF)
            {
                position++; // Simply advance, don't use consume() to avoid adding errors in this lookahead
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
            else // If EOF reached without finding '=' or '('
            {
                LogErrors::getInstance().addError("Unexpected EOF while parsing 'def' declaration. Missing '=' or '('?", 1, current().line, current().column);
                position = pos; // Restore position for better error context if needed, or simply return nullptr
                return nullptr; // Return nullptr for error progression
            }
        }

        default:
            // if the current type don't have the valid statements keywords,
            // throw a compilation exception
            LogErrors::getInstance().addError("Invalid statement/keyword '" + Tokens::tokenTypeToString(current().type) + "'", 2, current().line, current().column);
            return nullptr; // Return nullptr for error progression
        }
    }

    ASTPtr Parser::ParseFuncDeclaration()
    {
        // Check for nullptr returns from consume to ensure tokens are valid
        if (consume(TokensTypes::TOKEN_DEF).type != TokensTypes::TOKEN_DEF)
            return nullptr;
        std::string var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        if (var_name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
            return nullptr; // Check if identifier was consumed correctly
        if (consume(TokensTypes::TOKEN_COLON).type != TokensTypes::TOKEN_COLON)
            return nullptr;

        auto type_token = consume(current().type); // Consume the type token
        if (type_token.type == TokensTypes::TOKEN_EOF)
        { // Check if consume failed
            LogErrors::getInstance().addError("Expected a type token after ':' in function declaration.", 4, current().line, current().column);
            return nullptr;
        }
        TokensTypes type = type_token.type;

        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;
        std::vector<ASTPtr> args;

        while (!check(TokensTypes::TOKEN_RPAREN) && current().type != TokensTypes::TOKEN_EOF) // Added EOF check for robustness
        {
            ASTPtr arg = ParseFuncExpressions();
            if (!arg)
            {
                // Error occurred in parsing argument, attempt to recover by skipping to next comma or RPAREN
                while (!check(TokensTypes::TOKEN_COMMA) && !check(TokensTypes::TOKEN_RPAREN) && current().type != TokensTypes::TOKEN_EOF)
                {
                    position++;
                }
                if (current().type == TokensTypes::TOKEN_EOF)
                    return nullptr; // Return if EOF reached during recovery
                continue;           // Continue loop to try parsing next argument
            }
            args.push_back(arg);
            while (check(TokensTypes::TOKEN_COMMA))
            {
                consume(TokensTypes::TOKEN_COMMA);
                if (check(TokensTypes::TOKEN_RPAREN))
                { // Handle trailing comma error
                    LogErrors::getInstance().addError("Trailing comma in function arguments.", 4, current().line, current().column);
                    break; // Exit inner loop, next consume will be RPAREN
                }
                ASTPtr next_arg = ParseFuncExpressions();
                if (!next_arg)
                {
                    while (!check(TokensTypes::TOKEN_COMMA) && !check(TokensTypes::TOKEN_RPAREN) && current().type != TokensTypes::TOKEN_EOF)
                    {
                        position++;
                    }
                    if (current().type == TokensTypes::TOKEN_EOF)
                        return nullptr;
                    break; // Break inner loop, will check outer loop condition
                }
                args.push_back(next_arg);
            }
        }
        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        if (consume(TokensTypes::TOKEN_ARROW_SET).type != TokensTypes::TOKEN_ARROW_SET)
            return nullptr;

        auto block = ParseBlock();
        if (!block)
            return nullptr; // Error in parsing block
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
            if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
                return nullptr;
            val = ParseAdditiveExpression(); // Recursively parse the expression inside parentheses
            if (!val)
                return nullptr; // Error in recursive call
            if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
                return nullptr;
            break;
        case TokensTypes::TOKEN_IDENTIFIER: // Handle variable calls
            val = ParseVarCall();
            if (!val)
                return nullptr; // Error in variable call
            break;
        case TokensTypes::TOKEN_MINUS: // Handle unary minus
            if (consume(TokensTypes::TOKEN_MINUS).type != TokensTypes::TOKEN_MINUS)
                return nullptr;
            val = ParsePrimaryExpression(); // The operand of unary minus
            if (!val)
                return nullptr; // Error in operand
            val = std::make_shared<UnaryOp>(TokensTypes::TOKEN_MINUS, val);
            break;
        case TokensTypes::TOKEN_PLUS: // Handle unary plus (optional)
            if (consume(TokensTypes::TOKEN_PLUS).type != TokensTypes::TOKEN_PLUS)
                return nullptr;
            val = ParsePrimaryExpression(); // The operand of unary plus
            if (!val)
                return nullptr; // Error in operand
            val = std::make_shared<UnaryOp>(TokensTypes::TOKEN_PLUS, val);
            break;
        default:
            LogErrors::getInstance().addError("Expected a number, identifier, or '(' for expression", 197, current().line, current().column);
            return nullptr; // Return nullptr for error progression
        }
        return val;
    }

    // Parses multiplicative expressions (multiplication, division, modulo, bit_xor)
    // Uses left-to-right associativity and calls ParsePrimaryExpression for operands.
    ASTPtr Parser::ParseMultiplicativeExpression()
    {
        ASTPtr left = ParsePrimaryExpression();
        if (!left)
            return nullptr; // Error in left operand

        while (check(TokensTypes::TOKEN_MULTIPLY) || check(TokensTypes::TOKEN_DIVIDE) ||
               check(TokensTypes::TOKEN_MODULO) || check(TokensTypes::TOKEN_BIT_XOR)) // Added MODULO and BIT_XOR
        {
            TokensTypes op = consume(current().type).type; // consume should log error but not exit
            ASTPtr right = ParsePrimaryExpression();
            if (!right)
                return nullptr; // Error in right operand
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
        if (!left)
            return nullptr; // Error in left operand

        while (check(TokensTypes::TOKEN_PLUS) || check(TokensTypes::TOKEN_MINUS))
        {
            TokensTypes op = consume(current().type).type; // consume should log error but not exit
            ASTPtr right = ParseMultiplicativeExpression();
            if (!right)
                return nullptr; // Error in right operand
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
            val = std::make_shared<f64Node>(std::stod(consume(TokensTypes::TOKEN_FLOAT_64).value));
            break;
        default: // Added default case to catch non-numeral tokens
            LogErrors::getInstance().addError("Expected a numeral literal (int32, float32, etc.)", 198, current().line, current().column);
            // Removed LogErrors::getInstance().printAll(); and exit(198);
            return nullptr; // Return nullptr for error progression
        }
        return val;
    }

    ASTPtr Parser::ParseIfStatement()
    {
        // Ensure consume does not cause early exit
        if (consume(TokensTypes::TOKEN_IF).type != TokensTypes::TOKEN_IF)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        auto condition = ParseIfExpressions(); // conditional expression (e.g.: 1 == 2) or others
        if (!condition)
            return nullptr; // Error in condition parsing

        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        if (consume(TokensTypes::TOKEN_ARROW_SET).type != TokensTypes::TOKEN_ARROW_SET)
            return nullptr;

        auto ifBranch = ParseBlock(); // if body (ParseBlock handles brackets)
        if (!ifBranch)
            return nullptr; // Error in if branch parsing

        ASTPtr butBranch = nullptr;
        ASTPtr butCondition = nullptr;

        if (check(TokensTypes::TOKEN_BUT))
        {
            if (consume(TokensTypes::TOKEN_BUT).type != TokensTypes::TOKEN_BUT)
                return nullptr;
            if (check(TokensTypes::TOKEN_LPAREN))
            {
                if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
                    return nullptr;
                if (!check(TokensTypes::TOKEN_RPAREN)) // Check if not immediately closing
                {
                    butCondition = ParseIfExpressions();
                    if (!butCondition)
                        return nullptr; // Error in but condition parsing
                }
                if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
                    return nullptr;
            }
            if (consume(TokensTypes::TOKEN_ARROW_SET).type != TokensTypes::TOKEN_ARROW_SET)
                return nullptr;
            if (consume(TokensTypes::TOKEN_LBRACKET).type != TokensTypes::TOKEN_LBRACKET)
                return nullptr;       // ParseBlock already consumes LBRACKET, so this is redundant or indicative of a parse error. Keeping for now based on your previous code logic.
            butBranch = ParseBlock(); // 'but' body (ParseBlock handles brackets)
            if (!butBranch)
                return nullptr; // Error in but branch parsing
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
            if (exp_node->var_name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
                return nullptr; // Consume failed

            if (isConditionOperator(current().type)) // check if current token is a condition operator
            {
                exp_node->type = consume(current().type).type; // Consume the operator
                if (exp_node->type == TokensTypes::TOKEN_EOF)
                    return nullptr; // Consume failed

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
                    if (var->name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
                        return nullptr; // Consume failed
                    exp_node->val = var;
                    break;
                }
                default:
                    LogErrors::getInstance().addError("Expected a value or identifier after conditional operator", 200, current().line, current().column);
                    return nullptr;
                }
            }
            else
            {
                LogErrors::getInstance().addError("Expected a conditional operator (==, !=, >, etc.) after identifier in if expression", 201, current().line, current().column);
                return nullptr;
            }
        }
        else if (isTrueOrFalseOperator(current().type))
        {
            exp_node->type = consume(current().type).type; // Consumes TRUE or FALSE
            if (exp_node->type == TokensTypes::TOKEN_EOF)
                return nullptr; // Consume failed

            // If it's a direct boolean literal, val should likely be a TrueOrFalseNode directly.
            // Current setup uses 'type' for the boolean operator (TRUE/FALSE) which is fine for direct literals.
            exp_node->val = std::make_shared<TrueOrFalseNode>(exp_node->type == TokensTypes::TOKEN_TRUE);
        }
        else
        {
            LogErrors::getInstance().addError("Invalid expression in 'if' condition. Expected identifier or boolean literal.", 202, current().line, current().column);
            return nullptr;
        }
        return exp_node;
    }

    ASTPtr Parser::ParsePrint()
    {
        auto node = std::make_shared<PrintNode>();
        if (consume(TokensTypes::TOKEN_PRINT).type != TokensTypes::TOKEN_PRINT)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        if (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
                if (node->val.empty() && current().type != TokensTypes::TOKEN_STRING_LITERAL)
                    return nullptr; // Consume failed

                while (check(TokensTypes::TOKEN_PLUS))
                {
                    if (consume(TokensTypes::TOKEN_PLUS).type != TokensTypes::TOKEN_PLUS)
                        return nullptr;
                    // Ensure the next token is also a string literal for concatenation
                    if (check(TokensTypes::TOKEN_STRING_LITERAL))
                    {
                        node->val += consume(TokensTypes::TOKEN_STRING_LITERAL).value;
                    }
                    else if (check(TokensTypes::TOKEN_IDENTIFIER)) // Allows string concatenation with variable
                    {
                        // TODO: semantic analysis will need to verify if the identifier is a charseq
                        node->val += consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    }
                    else
                    {
                        LogErrors::getInstance().addError("Only string literals or identifiers can be concatenated with '+' in print for now.", 3, current().line, current().column);
                        return nullptr; // Return nullptr indicating an error in this AST subtree
                    }
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                if (consume(TokensTypes::TOKEN_NIL).type != TokensTypes::TOKEN_NIL)
                    return nullptr;
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
                consume(current().type);     // Consume the number token
            }
            else
            {
                LogErrors::getInstance().addError("Print statement supports string literals, identifiers, numbers, or 'nil'", 3, current().line, current().column);
                return nullptr;
            }
        }
        else
        {
            // Handle empty print()
            LogErrors::getInstance().addWarning("Empty print statement. Consider printing a newline with printnl()", 300, current().line, current().column);
        }
        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        return node;
    }

    ASTPtr Parser::ParseCinput()
    {
        if (consume(TokensTypes::TOKEN_CINPUT).type != TokensTypes::TOKEN_CINPUT)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        if (check(TokensTypes::TOKEN_STRING_LITERAL))
        {
            auto msg = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
            if (msg.empty() && current().type != TokensTypes::TOKEN_STRING_LITERAL)
                return nullptr; // Consume failed

            // The comma logic was inside a return which made it unreachable.
            // Re-structured to allow message and optional integer.
            if (check(TokensTypes::TOKEN_COMMA))
            {
                if (consume(TokensTypes::TOKEN_COMMA).type != TokensTypes::TOKEN_COMMA)
                    return nullptr;
                if (check(TokensTypes::TOKEN_INT_32))
                {
                    std::string int_val_str = consume(TokensTypes::TOKEN_INT_32).value;
                    if (int_val_str.empty() && current().type != TokensTypes::TOKEN_INT_32)
                        return nullptr; // Consume failed
                    if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
                        return nullptr;
                    return std::make_shared<CinputNode>(msg, std::stoi(int_val_str));
                }
                else
                {
                    LogErrors::getInstance().addError("Expected an integer after comma in cinput()", 301, current().line, current().column);
                    // Removed LogErrors::getInstance().printAll(); and exit(301);
                    return nullptr;
                }
            }
            else
            {
                // No comma, so only message provided
                if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
                    return nullptr;
                return std::make_shared<CinputNode>(msg);
            }
        }
        else
        {
            // If no string literal, assume cinput() with no arguments
            LogErrors::getInstance().addWarning("cinput() called without a message. Consider adding a prompt string.", 302, current().line, current().column);
        }

        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;                    // Consume final RPAREN for the no-arg case
        return std::make_shared<CinputNode>(); // Default cinput() if no specific arguments parsed (and no previous error)
    }

    ASTPtr Parser::ParsePrintE()
    {
        // TODO: remove the printe, print and cinput - This comment is from original code.
        auto node = std::make_shared<PrintE>();
        if (consume(TokensTypes::TOKEN_PRINT_ERROR).type != TokensTypes::TOKEN_PRINT_ERROR)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        if (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
                if (node->val.empty() && current().type != TokensTypes::TOKEN_STRING_LITERAL)
                    return nullptr; // Consume failed

                while (check(TokensTypes::TOKEN_PLUS))
                {
                    if (consume(TokensTypes::TOKEN_PLUS).type != TokensTypes::TOKEN_PLUS)
                        return nullptr;
                    if (check(TokensTypes::TOKEN_STRING_LITERAL))
                    {
                        node->val += consume(TokensTypes::TOKEN_STRING_LITERAL).value;
                    }
                    else if (check(TokensTypes::TOKEN_IDENTIFIER))
                    {
                        node->val += consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    }
                    else
                    {
                        LogErrors::getInstance().addError("Only string literals or identifiers can be concatenated with '+' in print_error for now.", 3, current().line, current().column);
                        return nullptr;
                    }
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                if (consume(TokensTypes::TOKEN_NIL).type != TokensTypes::TOKEN_NIL)
                    return nullptr;
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
                return nullptr;
            }
        }
        else
        {
            LogErrors::getInstance().addWarning("Empty print_error statement.", 303, current().line, current().column);
        }
        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        return node;
    }

    ASTPtr Parser::ParsePrintNl()
    {
        auto node = std::make_shared<PrintNl>();
        if (consume(TokensTypes::TOKEN_PRINT_NEW_LINE).type != TokensTypes::TOKEN_PRINT_NEW_LINE)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        if (!check(TokensTypes::TOKEN_RPAREN))
        {
            if (check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                node->val = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
                if (node->val.empty() && current().type != TokensTypes::TOKEN_STRING_LITERAL)
                    return nullptr; // Consume failed

                while (check(TokensTypes::TOKEN_PLUS))
                {
                    if (consume(TokensTypes::TOKEN_PLUS).type != TokensTypes::TOKEN_PLUS)
                        return nullptr;
                    if (check(TokensTypes::TOKEN_STRING_LITERAL))
                    {
                        node->val += consume(TokensTypes::TOKEN_STRING_LITERAL).value;
                    }
                    else if (check(TokensTypes::TOKEN_IDENTIFIER))
                    {
                        node->val += consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    }
                    else
                    {
                        LogErrors::getInstance().addError("Only string literals or identifiers can be concatenated with '+' in print_nl for now.", 3, current().line, current().column);
                        return nullptr;
                    }
                }
            }
            else if (check(TokensTypes::TOKEN_NIL))
            {
                if (consume(TokensTypes::TOKEN_NIL).type != TokensTypes::TOKEN_NIL)
                    return nullptr;
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
            else // Added else for comprehensive error handling
            {
                LogErrors::getInstance().addError("Print_nl statement supports string literals, identifiers, numbers, or 'nil'", 3, current().line, current().column);
                return nullptr;
            }
        }
        else
        {
            LogErrors::getInstance().addWarning("Empty print_nl statement.", 304, current().line, current().column);
        }
        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        return node;
    }

    // here we will parse the literal values or charseq values
    ASTPtr Parser::ParseCharseqValues()
    {
        // consume the predominant value
        //  it need be a charseq literal ("on quotes") or a identifier
        std::string val;

        val = consume(TokensTypes::TOKEN_STRING_LITERAL).value;
        
        if (check(TokensTypes::TOKEN_PLUS))
        {
            consume(TokensTypes::TOKEN_PLUS);
            val += consume(TokensTypes::TOKEN_STRING_LITERAL).value;

            // concatenation loop
            while (check(TokensTypes::TOKEN_PLUS))
            {
                if (consume(TokensTypes::TOKEN_PLUS).type != TokensTypes::TOKEN_PLUS)
                    return nullptr;
                
                // checks if is a variable call or a string literal
                switch (current().type)
                {
                    case TokensTypes::TOKEN_STRING_LITERAL:
                        val += consume(TokensTypes::TOKEN_STRING_LITERAL).value;       
                        break;
                    case TokensTypes::TOKEN_IDENTIFIER:
                        break;
                    default:
                        LogErrors::getInstance().addError("Invalid value for a charseq type", 51, current().line, current().column);
                        return nullptr;
                }
            }
        }

        return std::make_shared<LiteralNode>(val);
    }

    ASTPtr Parser::ParseVarCall()
    {
        auto var_node = std::make_shared<VariableNode>();
        var_node->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        if (var_node->name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
            return nullptr; // Consume failed
        return var_node;
    }

    ASTPtr Parser::ParseVarDeclaration()
    {
        if (consume(TokensTypes::TOKEN_DEF).type != TokensTypes::TOKEN_DEF)
            return nullptr;
        std::string name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        if (name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
            return nullptr; // Consume failed
        if (consume(TokensTypes::TOKEN_COLON).type != TokensTypes::TOKEN_COLON)
            return nullptr;

        TokensTypes tk;
        Tokens type_token = consume(current().type);
        if (type_token.type == TokensTypes::TOKEN_EOF)
        { // Check if consume failed
            LogErrors::getInstance().addError("Expected a type token after ':' in variable declaration.", 54, current().line, current().column);
            return nullptr;
        }
        tk = type_token.type;

        // consume the ':=' to get the value
        if (consume(TokensTypes::TOKEN_ASSIGN).type != TokensTypes::TOKEN_ASSIGN)
            return nullptr;

        // parse value after assign based on the defined type on tk
        auto val = ParseExpression(tk);
        if (!val)
            return nullptr; // Error in parsing expression
        return std::make_shared<VariableDefinitionNode>(name, tk, val);
    }

    ASTPtr Parser::ParseFuncExpressions()
    {
        auto exp_node = std::make_shared<ExpressionNode>();
        if (!check(TokensTypes::TOKEN_IDENTIFIER))
        { // Ensure identifier is present
            LogErrors::getInstance().addError("Expected identifier for function argument name", 90, current().line, current().column);
            return nullptr;
        }
        exp_node->var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        if (exp_node->var_name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
            return nullptr; // Consume failed
        if (consume(TokensTypes::TOKEN_COLON).type != TokensTypes::TOKEN_COLON)
            return nullptr;

        Tokens type_token = consume(current().type);
        if (type_token.type == TokensTypes::TOKEN_EOF)
        { // Check if consume failed
            LogErrors::getInstance().addError("Expected a type token after ':' in function argument definition.", 91, current().line, current().column);
            return nullptr;
        }
        exp_node->type = type_token.type;

        return exp_node;
    }

    ASTPtr Parser::ParseExpression(TokensTypes types)
    {
        // consume the value based on the type of the variable
        // this helps the semantic analysis
        ASTPtr parsed_val = nullptr; // Initialize parsed_val

        switch (types)
        {
        case TokensTypes::TOKEN_CHARSEQ:
            if (!check(TokensTypes::TOKEN_STRING_LITERAL))
            {
                LogErrors::getInstance().addError("Expected a string literal for 'charseq' type", 98, current().line, current().column);
                return nullptr;
            }
            parsed_val = ParseCharseqValues();
            break;
        // case TokensTypes::TOKEN_PLUS: // This case handles unary plus at the start of an expression
        //     if (consume(TokensTypes::TOKEN_PLUS).type != TokensTypes::TOKEN_PLUS) return nullptr;
        //     parsed_val = ParsePositiveVals(current().type); // ParsePositiveVals now expects the type to consume
        //     break;
        // case TokensTypes::TOKEN_MINUS: // This case handles unary minus at the start of an expression
        //     if (consume(TokensTypes::TOKEN_MINUS).type != TokensTypes::TOKEN_MINUS) return nullptr;
        //     parsed_val = ParseNegativeVals(current().type); // ParseNegativeVals now expects the type to consume
        //     break;
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            parsed_val = ParseAdditiveExpression(); // Now calls the top-level arithmetic expression parser
            break;
        case TokensTypes::TOKEN_BYTES:
            parsed_val = ParseByteVal();
            break;
        case TokensTypes::TOKEN_BOOL:
            parsed_val = ParseLoopCondition(); // ParseLoopCondition returns a boolean node (true/false literal)
            break;
        case TokensTypes::TOKEN_OBJECT:
            switch (current().type)
            {
            case TokensTypes::TOKEN_INT_32:
            case TokensTypes::TOKEN_INT_64:
            case TokensTypes::TOKEN_FLOAT_32:
            case TokensTypes::TOKEN_FLOAT_64:
                parsed_val = std::make_shared<ObjectNode>(ParseNumeralExpression());
                break;
            case TokensTypes::TOKEN_TRUE:
            {
                if (consume(TokensTypes::TOKEN_TRUE).type != TokensTypes::TOKEN_TRUE)
                    return nullptr;
                auto ptr = std::make_shared<TrueOrFalseNode>(true);
                parsed_val = std::make_shared<ObjectNode>(ptr);
                break;
            }
            case TokensTypes::TOKEN_FALSE:
            {
                if (consume(TokensTypes::TOKEN_FALSE).type != TokensTypes::TOKEN_FALSE)
                    return nullptr;
                auto ptr = std::make_shared<TrueOrFalseNode>(false);
                parsed_val = std::make_shared<ObjectNode>(ptr);
                break;
            }
            case TokensTypes::TOKEN_STRING_LITERAL:
            {
                auto ptr = ParseCharseqValues();
                if (!ptr)
                    return nullptr; // consume might return an invalid token or error
                parsed_val = std::make_shared<ObjectNode>(ptr);
                break;
            }
            case TokensTypes::TOKEN_IDENTIFIER:
            {
                ASTPtr ptr;
                if (peek(1).type == TokensTypes::TOKEN_LPAREN) // Check if it's a function call
                {
                    auto id = std::make_shared<IdentifierNode>(); // Assuming IdentifierNode for function calls with args
                    id->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    if (id->name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
                        return nullptr; // Consume failed

                    if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
                        return nullptr;
                    while (!check(TokensTypes::TOKEN_RPAREN) && current().type != TokensTypes::TOKEN_EOF) // Added EOF check
                    {
                        // Parse arguments for function call. Needs to handle various types.
                        ASTPtr arg_val = nullptr;
                        switch (current().type)
                        {
                        case TokensTypes::TOKEN_IDENTIFIER:
                        {
                            // If it's just an identifier (variable passed as arg)
                            auto var = std::make_shared<VariableNode>();
                            var->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                            if (var->name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
                                return nullptr; // Consume failed
                            arg_val = var;
                            break;
                        }
                        case TokensTypes::TOKEN_INT_32:
                        case TokensTypes::TOKEN_INT_64:
                        case TokensTypes::TOKEN_FLOAT_32:
                        case TokensTypes::TOKEN_FLOAT_64:
                            arg_val = ParseNumeralExpression();
                            break;
                        case TokensTypes::TOKEN_TRUE:
                        case TokensTypes::TOKEN_FALSE:
                            arg_val = ParseLoopCondition(); // TrueOrFalseNode
                            break;
                        case TokensTypes::TOKEN_STRING_LITERAL: // Allow string literals as arguments
                            arg_val = std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value);
                            break;
                        default:
                            LogErrors::getInstance().addError("Invalid argument type in function call", 96, current().line, current().column);
                            // Attempt to skip this invalid argument to continue parsing
                            position++;        // Simple skip
                            arg_val = nullptr; // Explicitly set to nullptr to indicate parse error
                            break;
                        }
                        if (arg_val)
                        { // Only add if parsing was successful
                            id->args.push_back(arg_val);
                        }
                        else
                        {
                            // If arg_val is nullptr, an error was logged inside the switch.
                            // We need to decide how to recover for the next argument or close RPAREN.
                            // A robust parser might skip tokens until the next COMMA or RPAREN.
                            while (!check(TokensTypes::TOKEN_COMMA) && !check(TokensTypes::TOKEN_RPAREN) && current().type != TokensTypes::TOKEN_EOF)
                            {
                                position++;
                            }
                            if (current().type == TokensTypes::TOKEN_EOF)
                                return nullptr; // Return if EOF
                            if (current().type == TokensTypes::TOKEN_COMMA)
                                continue; // Try next arg
                            if (current().type == TokensTypes::TOKEN_RPAREN)
                                break; // End of arguments
                        }

                        // Handle multiple arguments separated by commas (if your language supports it)
                        if (check(TokensTypes::TOKEN_COMMA))
                        {
                            if (consume(TokensTypes::TOKEN_COMMA).type != TokensTypes::TOKEN_COMMA)
                                return nullptr;
                        }
                        else if (!check(TokensTypes::TOKEN_RPAREN))
                        { // If not comma and not end of args, then error
                            LogErrors::getInstance().addError("Expected comma or ')' after function argument", 97, current().line, current().column);
                            return nullptr;
                        }
                    }
                    if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
                        return nullptr;
                    ptr = id;
                }
                else
                {
                    // It's a variable call
                    auto id = std::make_shared<VariableNode>();
                    id->name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
                    if (id->name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
                        return nullptr; // Consume failed
                    ptr = id;
                }
                parsed_val = std::make_shared<ObjectNode>(ptr);
                break;
            }
            default:
                LogErrors::getInstance().addError("Invalid variable value for 'obj' type", 95, current().line, current().column);
                return nullptr;
            }
            break; // Break from the inner switch (TOKEN_OBJECT)
        default:
            LogErrors::getInstance().addError("Invalid variable value type", 97, current().line, current().column);
            return nullptr;
            // throw Excepts::CompilationException("Invalid Variable Value Type"); // This line was problematic
        }
        return parsed_val; // Return the parsed value or nullptr on error
    }

    // needs a correction for perfectly work

    ASTPtr Parser::ParseLoopCondition()
    {
        // This function now just parses true/false literals, suitable for boolean expressions.
        if (current().type == TokensTypes::TOKEN_TRUE)
        {
            if (consume(TokensTypes::TOKEN_TRUE).type != TokensTypes::TOKEN_TRUE)
                return nullptr;
            return std::make_shared<TrueOrFalseNode>(true);
        }
        else if (current().type == TokensTypes::TOKEN_FALSE)
        {
            if (consume(TokensTypes::TOKEN_FALSE).type != TokensTypes::TOKEN_FALSE)
                return nullptr;
            return std::make_shared<TrueOrFalseNode>(false);
        }
        else
        {
            LogErrors::getInstance().addError("Expected 'true' or 'false' for loop condition literal.", 203, current().line, current().column);
            return nullptr;
        }
    }

    ASTPtr Parser::ParseLoopExpression()
    {
        if (consume(TokensTypes::TOKEN_LOOP).type != TokensTypes::TOKEN_LOOP)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        std::string var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        if (var_name.empty() && current().type != TokensTypes::TOKEN_IDENTIFIER)
            return nullptr; // Consume failed

        // consome o : para em seguida consumir o tipo da variavel
        if (consume(TokensTypes::TOKEN_COLON).type != TokensTypes::TOKEN_COLON)
            return nullptr;

        TokensTypes type;
        Tokens type_token = consume(current().type);
        if (type_token.type == TokensTypes::TOKEN_EOF)
        { // Check if consume failed
            LogErrors::getInstance().addError("Expected a type token after ':' in loop expression variable definition.", 23, current().line, current().column);
            return nullptr;
        }
        type = type_token.type;

        if (consume(TokensTypes::TOKEN_IN).type != TokensTypes::TOKEN_IN)
            return nullptr;
        ASTPtr val;
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            val = ParseNumeralExpression();
            if (!val)
                return nullptr; // Error in parsing numeral expression
            break;
        case TokensTypes::TOKEN_IDENTIFIER: // Allows loop over a variable (e.g., an array)
            val = ParseVarCall();
            if (!val)
                return nullptr; // Error in variable call
            break;
        default:
            LogErrors::getInstance().addError("Invalid type for 'in' value in loop expression. Expected numeral or a identifier.", 23, current().line, current().column);
            return nullptr;
        }
        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        if (consume(TokensTypes::TOKEN_ARROW_SET).type != TokensTypes::TOKEN_ARROW_SET)
            return nullptr;

        auto block = ParseBlock();
        if (!block)
            return nullptr; // Error in parsing block
        return std::make_shared<LoopNode>(var_name, type, val, block);
    }

    ASTPtr Parser::ParseLoopCond()
    {
        if (consume(TokensTypes::TOKEN_LOOP).type != TokensTypes::TOKEN_LOOP)
            return nullptr;
        if (consume(TokensTypes::TOKEN_LPAREN).type != TokensTypes::TOKEN_LPAREN)
            return nullptr;

        auto node = std::make_shared<LoopConditionNode>();

        ASTPtr condition_node = nullptr;
        switch (current().type)
        {
        case TokensTypes::TOKEN_FALSE:
        case TokensTypes::TOKEN_TRUE:
            condition_node = ParseLoopCondition(); // Parses boolean literal (true/false)
            break;
        case TokensTypes::TOKEN_IDENTIFIER:
            condition_node = ParseVarCall();
            if (!condition_node)
                return nullptr; // Error in variable call
            break;
        // Added support for direct arithmetic/comparison expressions as loop conditions (e.g., loop (10 > x) ->)
        case TokensTypes::TOKEN_INT_32:
        case TokensTypes::TOKEN_INT_64:
        case TokensTypes::TOKEN_FLOAT_32:
        case TokensTypes::TOKEN_FLOAT_64:
            // If a number starts the expression, try to parse it as an arithmetic/comparison expression
            condition_node = ParseIfExpressions(); // Reusing ParseIfExpressions might work if it supports direct comparisons without an initial IDENTIFIER
            break;
        default:
            LogErrors::getInstance().addError("Invalid token for loop condition. Expected boolean literal, identifier, or expression.", 204, current().line, current().column);
            return nullptr;
        }

        if (!condition_node)
            return nullptr; // If condition parsing failed
        node->condition = condition_node;

        if (consume(TokensTypes::TOKEN_RPAREN).type != TokensTypes::TOKEN_RPAREN)
            return nullptr;
        if (consume(TokensTypes::TOKEN_ARROW_SET).type != TokensTypes::TOKEN_ARROW_SET)
            return nullptr;

        node->body = ParseBlock();
        if (!node->body)
            return nullptr; // Error in parsing block

        return node;
    }

    ASTPtr Parser::ParseByteVal()
    {
        long long lit_val;
        unsigned char val;

        // Ensure consume returns a valid token before accessing its value
        Tokens num_token = current(); // Get current token before consuming
        if (num_token.type == TokensTypes::TOKEN_INT_32 || num_token.type == TokensTypes::TOKEN_INT_64 ||
            num_token.type == TokensTypes::TOKEN_FLOAT_32 || num_token.type == TokensTypes::TOKEN_FLOAT_64)
        {
            // Assuming we only take the integer part for byte conversion
            lit_val = std::stoll(consume(num_token.type).value);
        }
        else
        {
            LogErrors::getInstance().addError("Expected a number literal for byte type", 88, current().line, current().column);
            // Removed LogErrors::getInstance().printAll(); and exit(88);
            return nullptr; // Return nullptr for error progression
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

        return std::make_shared<ByteNode>(val);
    }

    ASTPtr Parser::ParseBlock()
    {
        auto block = std::make_shared<BlockNode>();
        if (consume(TokensTypes::TOKEN_LBRACKET).type != TokensTypes::TOKEN_LBRACKET)
            return nullptr; // '['

        while (!check(TokensTypes::TOKEN_RBRACKET))
        {
            if (current().type == TokensTypes::TOKEN_EOF)
            {
                LogErrors::getInstance().addError("Unclosed block. Expected ']' but reached end of file.", 57, current().line, current().column);
                // Removed LogErrors::getInstance().printAll(); and exit(57);
                return nullptr; // Return nullptr for error progression
            }
            ASTPtr statement = ParseDeclarations();
            if (statement)
            {
                block->statements.push_back(statement);
            }
            else
            {
                // Error in parsing a statement within the block, attempt to synchronize
                // by skipping tokens until a known statement start or block end.
                // This is a simple recovery, might need more sophisticated skipping.
                while (!check(TokensTypes::TOKEN_RBRACKET) &&
                       current().type != TokensTypes::TOKEN_EOF &&
                       current().type != TokensTypes::TOKEN_CINPUT &&
                       current().type != TokensTypes::TOKEN_IF &&
                       current().type != TokensTypes::TOKEN_LOOP &&
                       current().type != TokensTypes::TOKEN_PRINT &&
                       current().type != TokensTypes::TOKEN_PRINT_ERROR &&
                       current().type != TokensTypes::TOKEN_PRINT_NEW_LINE &&
                       current().type != TokensTypes::TOKEN_DEF &&
                       current().type != TokensTypes::TOKEN_STRING_LITERAL) // Added STRING_LITERAL for recovery
                {
                    position++; // Skip current token
                }
                // If we skipped to EOF, or if the next token is RBRACKET, we can break or proceed.
                if (current().type == TokensTypes::TOKEN_EOF)
                {
                    return nullptr; // Unclosed block reaching EOF
                }
            }
        }
        if (consume(TokensTypes::TOKEN_RBRACKET).type != TokensTypes::TOKEN_RBRACKET)
            return nullptr; // ']'
        return block;
    }
}
