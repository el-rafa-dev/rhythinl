#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <optional>

// local includes
#include "../src/includes/ast.h"
#include "../src/includes/t_tokens.h"
#include "../src/includes/r_parser.h"
#include "../src/includes/r_opcodes.h"
#include "../src/includes/rexcept.h"
#include "../src/includes/chunk.h"
#include "../src/includes/log.h"

// using namespace std;
using namespace Log;

namespace Rythin
{

    Parser::Parser(std::vector<Tokens> tokens) : position(0), tokens(tokens)
    {
    }

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
        if (current().type == TokensTypes::TOKEN_EOF)
        {
            // checa se o token atual é o EOF (end of file), se for retorna o erro anaixo
            LogErrors::getInstance().addError("Expected a statement but reached the end of file.... Are you forget anything?", 1);
            throw Excepts::CompilationException("Left early");
        }

        if (current().type != tk)
        {
            // checa se o tipo não é igual ao tk e retorna  o erro abaixo
            LogErrors::getInstance().addError("Expected '" + Tokens::tokenTypeToString(tk) + "' but got: '" + Tokens::tokenTypeToString(current().type) + "' at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 4);
            LogErrors::getInstance().printAll();
            throw Excepts::CompilationException("Invalid token");
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

    //look the current type
    bool Parser::check(TokensTypes tk)
    {
        return current().type == tk;
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
            return ParseLoopExpression();
        case TokensTypes::TOKEN_PRINT:
            return ParsePrint();
        case TokensTypes::TOKEN_PRINT_ERROR:
            return ParsePrintE();
        case TokensTypes::TOKEN_PRINT_NEW_LINE:
            return ParsePrintNl();
        case TokensTypes::TOKEN_DEF:
            return ParseFuncDeclaration();
        case TokensTypes::TOKEN_LET:
            return ParseVarDeclaration();
        default:
            // only for debug of errors
            std::cout << "The invalid statement " << Tokens::tokenTypeToString(current().type) << std::endl;
            LogErrors::getInstance().addError("Invalid statement at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 2);
            LogErrors::getInstance().printAll();
            throw Excepts::CompilationException("Invalid Statement");
        }
    }

    ASTPtr Parser::ParseFuncDeclaration()
    {
        consume(TokensTypes::TOKEN_DEF);
        std::string var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        consume(TokensTypes::TOKEN_COLON);
        auto type = consume(TokensTypes::TOKEN_FUNC).type;
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
        auto block = ParseBlock();
        return std::make_shared<FunctionDefinitionNode>(var_name, args, block);
    }

    ASTPtr Parser::ParseIntVal() {
    
    try
        {
            auto int_val = std::stoi(consume(TokensTypes::TOKEN_INT).value);
            //check if have a binary operator (like +, /, -, *) after int value;
            if (isBinaryOperator(current().type)) {
                //TODO: add more binary types for the switch cases
                switch (consume(current().type).type)
                {
                    case TokensTypes::TOKEN_PLUS:
                        if (check(TokensTypes::TOKEN_INT)) return std::make_shared<IntNode>(int_val + std::stoi(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_FLOAT)) return std::make_shared<IntNode>(int_val + std::stof(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_DOUBLE)) return std::make_shared<IntNode>(int_val + std::stod(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_LPAREN)) 
                        consume(TokensTypes::TOKEN_LPAREN);
                        do {
                            //return ParseIntVal();
                            return std::make_shared<IntNode>(int_val + ParsedArith());
                        } while (!check(TokensTypes::TOKEN_RPAREN));
                        consume(TokensTypes::TOKEN_RPAREN);
                    case TokensTypes::TOKEN_MINUS:
                        if (check(TokensTypes::TOKEN_INT)) return std::make_shared<IntNode>(int_val - std::stoi(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_FLOAT)) return std::make_shared<IntNode>(int_val - std::stof(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_DOUBLE)) return std::make_shared<IntNode>(int_val - std::stod(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_LPAREN))                         
                        consume(TokensTypes::TOKEN_LPAREN);
                        do {
                            return std::make_shared<IntNode>(int_val - ParsedArith());
                        } while (!check(TokensTypes::TOKEN_RPAREN));
                        consume(TokensTypes::TOKEN_RPAREN);
                    case TokensTypes::TOKEN_DIVIDE:
                        if (check(TokensTypes::TOKEN_INT)) return std::make_shared<IntNode>(int_val * std::stoi(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_FLOAT)) return std::make_shared<IntNode>(int_val * std::stof(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_DOUBLE)) return std::make_shared<IntNode>(int_val * std::stod(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_LPAREN))                        
                        consume(TokensTypes::TOKEN_LPAREN);
                        do {
                            return std::make_shared<IntNode>(int_val / ParsedArith());
                        } while (!check(TokensTypes::TOKEN_RPAREN));
                        consume(TokensTypes::TOKEN_RPAREN);
                        break;
                    case TokensTypes::TOKEN_MULTIPLY:
                        if (check(TokensTypes::TOKEN_INT)) return std::make_shared<IntNode>(int_val * std::stoi(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_FLOAT)) return std::make_shared<IntNode>(int_val * std::stof(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_DOUBLE)) return std::make_shared<IntNode>(int_val * std::stod(consume(current().type).value));
                        else if (check(TokensTypes::TOKEN_LPAREN)) 
                        consume(TokensTypes::TOKEN_LPAREN);
                        do {
                            return std::make_shared<IntNode>(int_val * ParsedArith());
                        } while (!check(TokensTypes::TOKEN_RPAREN));
                        consume(TokensTypes::TOKEN_RPAREN);
                }
            }
            return std::make_shared<IntNode>(int_val);
        }
        catch (std::out_of_range e)
        {
            std::cerr << "[Error]: Current value out of range at line " << current().line << " column " << current().column << std::endl;
            throw std::out_of_range("Index out of range");
        }
    }

    int Parser::ParsedArith() 
    {
        int val = std::stoi(consume(current().type).value);
        if (isBinaryOperator(current().type))
        {
            switch (consume(current().type).type)
            {
                case TokensTypes::TOKEN_PLUS:
                    return val + std::stoi(consume(current().type).value);
                case TokensTypes::TOKEN_MINUS:
                    return val - std::stoi(consume(current().type).value);
                case TokensTypes::TOKEN_DIVIDE:
                    return val / std::stoi(consume(current().type).value);
                case TokensTypes::TOKEN_MULTIPLY:
                    return val * std::stoi(consume(current().type).value);
                case TokensTypes::TOKEN_MODULO:
                    return val % std::stoi(consume(current().type).value);
                case TokensTypes::TOKEN_BIT_XOR:
                    return val ^ std::stoi(consume(current().type).value);
                default:
                    LogErrors::getInstance().addError("Invalid type for arithmetic expression at line" + std::to_string(current().line) + " column " + std::to_string(current().column), 23);
                    LogErrors::getInstance().printAll();
                    throw Excepts::SyntaxException("Invalid expression");
            }
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
                    case TokensTypes::TOKEN_INT:
                        exp_node->val = std::make_shared<IntNode>(std::stoi(consume(current().type).value));
                        break;
                    case TokensTypes::TOKEN_FLOAT:
                        exp_node->val = std::make_shared<FloatNode>(std::stof(consume(current().type).value));
                        break;
                    case TokensTypes::TOKEN_DOUBLE:
                        exp_node->val = std::make_shared<DoubleNode>(std::stod(consume(current().type).value));
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
                LogErrors::getInstance().addError("Concatenation needs a predominant value (like str, int or others types)", 3);
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
                return std::make_shared<CinputNode>(msg, std::stoi(consume(TokensTypes::TOKEN_INT).value));
            }
        }
        consume(TokensTypes::TOKEN_RPAREN);
        return std::make_shared<CinputNode>();
    }

    ASTPtr Parser::ParsePrintE()
    {
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
                LogErrors::getInstance().addError("Concatenation needs a predominant value to concats with other values types", 3);
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

    ASTPtr Parser::ParseVarDeclaration()
    {
        consume(TokensTypes::TOKEN_LET);
        std::string name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        consume(TokensTypes::TOKEN_COLON);
        TokensTypes tk;
        switch (current().type)
        {
        case TokensTypes::TOKEN_BOOL:
            tk = consume(TokensTypes::TOKEN_BOOL).type;
            break;
        case TokensTypes::TOKEN_INT:
            tk = consume(TokensTypes::TOKEN_INT).type;
            break;
        case TokensTypes::TOKEN_FLOAT:
            tk = consume(TokensTypes::TOKEN_FLOAT).type;
            break;
        case TokensTypes::TOKEN_DOUBLE:
            tk = consume(TokensTypes::TOKEN_DOUBLE).type;
            break;
        case TokensTypes::TOKEN_STR:
            tk = consume(TokensTypes::TOKEN_STR).type;
            break;
        case TokensTypes::TOKEN_BYTES:
            tk = consume(TokensTypes::TOKEN_BYTES).type;
            break;
        case TokensTypes::TOKEN_OBJECT:
            tk = consume(TokensTypes::TOKEN_OBJECT).type;
            break;
        case TokensTypes::TOKEN_LONG_INT:
            tk = consume(TokensTypes::TOKEN_LONG_INT).type;
            break;
        default:
            std::cerr << "[Error] Invalid type for variable definition at line " << current().line << " column " << current().column << std::endl;
            throw Excepts::CompilationException("Invalid type definition");
        }
        consume(TokensTypes::TOKEN_ASSIGN); // consome o '=' para pegar o valor ou expressão
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
        switch (types)
        {
        case TokensTypes::TOKEN_STR:
            return std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value);
        case TokensTypes::TOKEN_PLUS:
            consume(TokensTypes::TOKEN_PLUS);
            return ParsePositiveVals(consume(current().type).type);
        case TokensTypes::TOKEN_MINUS:
            consume(TokensTypes::TOKEN_MINUS);
            return ParseNegativeVals(consume(current().type).type);
        case TokensTypes::TOKEN_INT:
        {
            return ParseIntVal();
            /**/
        }
        case TokensTypes::TOKEN_FLOAT:
            if (check(TokensTypes::TOKEN_FLOAT))
            {
                return std::make_shared<FloatNode>(std::stof(consume(TokensTypes::TOKEN_FLOAT).value));
            }
            else if (check(TokensTypes::TOKEN_DOUBLE))
            {
                LogErrors::getInstance().addWarning("Using double as float at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 6);
                return std::make_shared<FloatNode>(std::stof(consume(TokensTypes::TOKEN_DOUBLE).value));
            }

        case TokensTypes::TOKEN_DOUBLE:
            if (check(TokensTypes::TOKEN_DOUBLE))
            {
                return std::make_shared<DoubleNode>(std::stod(consume(TokensTypes::TOKEN_DOUBLE).value));
            }
            else if (check(TokensTypes::TOKEN_FLOAT))
            {
                LogErrors::getInstance().addWarning("Using float as double at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 7);
                return std::make_shared<DoubleNode>(std::stod(consume(TokensTypes::TOKEN_FLOAT).value));
            }
        case TokensTypes::TOKEN_BYTES:
        {
            auto lit_val = std::stoll(current().value);
            unsigned char val;
            if (lit_val < 0 || lit_val > 255)
            {
                LogErrors::getInstance().addWarning("Value " + std::to_string(lit_val) + " is out of range for byte type. It will be truncated to: " + std::to_string(static_cast<unsigned char>(lit_val)), 5);
                val = static_cast<unsigned char>(lit_val);
            }
            else
            {
                val = static_cast<unsigned char>(lit_val);
            }
            if (!check(TokensTypes::TOKEN_INT))
                LogErrors::getInstance().addError("", 26);
            consume(TokensTypes::TOKEN_INT);
            return std::make_shared<ByteNode>(val);
        }
        case TokensTypes::TOKEN_LONG_INT:
            if (std::stoll(current().value) < (-9223372036854775807) || std::stoll(current().value) > (9223372036854775807))
            {
                std::cerr << "[Warning]: Value " << std::stoll(current().value) << " out of range at line " << current().line << " column " << current().column << std::endl;
                throw Excepts::CompilationException("Index out of range");
            }
            else
            {
                return std::make_shared<LIntNode>(std::stoll(consume(current().type).value));
            }

        case TokensTypes::TOKEN_NIL:
            consume(TokensTypes::TOKEN_NIL);
            return std::make_shared<NilNode>();
        case TokensTypes::TOKEN_OBJECT:
            if (check(TokensTypes::TOKEN_INT))
            {
                auto ptr = std::make_shared<IntNode>(std::stoi(consume(current().type).value));
                return std::make_shared<ObjectNode>(ptr);
            }
            else if (check(TokensTypes::TOKEN_DOUBLE))
            {
                auto ptr = std::make_shared<DoubleNode>(std::stod(consume(current().type).value));
                return std::make_shared<ObjectNode>(ptr);
            }
            else if (check(TokensTypes::TOKEN_STR))
            {
                auto ptr = std::make_shared<LiteralNode>(consume(current().type).value);
                return std::make_shared<ObjectNode>(ptr);
            }
            else if (check(TokensTypes::TOKEN_FLOAT))
            {
                auto ptr = std::make_shared<FloatNode>(std::stof(consume(current().type).value));
                return std::make_shared<ObjectNode>(ptr);
            }
        case TokensTypes::TOKEN_BOOL:
            if (check(TokensTypes::TOKEN_TRUE))
            {
                consume(TokensTypes::TOKEN_TRUE);
                return std::make_shared<TrueOrFalseNode>(true);
            }
            else if (check(TokensTypes::TOKEN_FALSE))
            {
                consume(TokensTypes::TOKEN_FALSE);
                return std::make_shared<TrueOrFalseNode>(false);
            }
        default:
            std::cerr << "[Error]: Invalid variable value type at line " << current().line << " column " << current().column << std::endl;
            throw Excepts::CompilationException("Invalid Variable Value Type");
        }
    }

    ASTPtr Parser::ParsePositiveVals(TokensTypes curr)
    {
        switch (curr)
        {
        case TokensTypes::TOKEN_INT:
            try
            {
                return std::make_shared<IntNode>(+std::stoi(consume(TokensTypes::TOKEN_INT).value));
            }
            catch (std::out_of_range e)
            {
                std::cerr << "[Error]: Current value out of range at line " << current().line << " column " << current().column << std::endl;
                throw std::out_of_range("Index out of range");
            }

        case TokensTypes::TOKEN_DOUBLE:
            return std::make_shared<DoubleNode>(+std::stod(consume(current().type).value));
        case TokensTypes::TOKEN_FLOAT:
            return std::make_shared<FloatNode>(+std::stof(consume(current().type).value));
        default:
            throw Excepts::CompilationException("Invalid positive type");
        }
    }

    ASTPtr Parser::ParseNegativeVals(TokensTypes tks)
    {
        switch (tks)
        {
        case TokensTypes::TOKEN_INT:
            try
            {
                return std::make_shared<IntNode>(-std::stoi(consume(TokensTypes::TOKEN_INT).value));
            }
            catch (std::out_of_range e)
            {
                std::cerr << "[Error]: Current value out of range at line " << current().line << " column " << current().column << std::endl;
                throw std::out_of_range("Index out of range");
            }

        case TokensTypes::TOKEN_DOUBLE:
            return std::make_shared<DoubleNode>(-std::stod(consume(current().type).value));
        case TokensTypes::TOKEN_FLOAT:
            return std::make_shared<FloatNode>(-std::stof(consume(current().type).value));
        default:
            throw Excepts::CompilationException("Invalid positive type");
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
        if (check(TokensTypes::TOKEN_TRUE))
        {
            return ParseLoopCond();
        }
        else if (check(TokensTypes::TOKEN_FALSE))
        {
            return ParseLoopCond();
        }
        auto var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        // consome o : para em seguida consumir o tipo da variavel
        consume(TokensTypes::TOKEN_COLON);
        // switch para verificação de tipos da variavel
        TokensTypes type;
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT:
            type = consume(TokensTypes::TOKEN_INT).type;
            break;
        case TokensTypes::TOKEN_FLOAT:
            type = consume(TokensTypes::TOKEN_FLOAT).type;
            break;
        case TokensTypes::TOKEN_DOUBLE:
            type = consume(TokensTypes::TOKEN_DOUBLE).type;
            break;
        default:
            LogErrors::getInstance().addError("Loop expression only accepts number types (int, float, double)", 23);
            throw Excepts::SyntaxException("Loop Expression");
        }
        consume(TokensTypes::TOKEN_IN);
        ASTPtr val;
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT:
            val = std::make_shared<IntNode>(std::stoi(consume(TokensTypes::TOKEN_INT).value));
            break;
        case TokensTypes::TOKEN_FLOAT:
            val = std::make_shared<FloatNode>(std::stof(consume(TokensTypes::TOKEN_FLOAT).value));
            break;
        case TokensTypes::TOKEN_DOUBLE:
            val = std::make_shared<DoubleNode>(std::stod(consume(TokensTypes::TOKEN_DOUBLE).value));
            break;
        default:
            LogErrors::getInstance().addError("Invalid type for loop expression", 23);
            throw Excepts::SyntaxException("Invalid loop type");
        }
        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET);
        auto block = ParseBlock();
        return std::make_shared<LoopNode>(var_name, type, val, block);
    }

    ASTPtr Parser::ParseLoopCond()
    {
        auto node = std::make_shared<LoopConditionNode>();
        /// ParseLoopExpression will be check if have a varaible definition and his type or condition
        node->condition = ParseLoopCondition();

        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET);

        node->body = ParseBlock();

        return node;
    }

    ASTPtr Parser::ParseBlock()
    {
        auto block = std::make_shared<BlockNode>();
        consume(TokensTypes::TOKEN_LBRACKET); // '['

        while (!check(TokensTypes::TOKEN_RBRACKET))
        {
            block->statements.push_back(ParseDeclarations());
        }

        consume(TokensTypes::TOKEN_RBRACKET); // ']'
        return block;
    }
}