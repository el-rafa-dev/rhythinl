#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <optional>

// local includes
#include "ast.h"
#include "t_tokens.h"
#include "r_parser.h"
#include "r_opcodes.h"
#include "rexcept.h"
#include "chunk.h"
#include "log.h"

// using namespace std;
using namespace Log;

namespace Rythin
{

    Parser::Parser(std::vector<Tokens> tokens, std::string current_file) : position(0), tokens(tokens), current_file(current_file)
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
            position++;
        }
        position++;
        return tokens[position - 1];
        // std::cout << "current token: " << tokens[position].value << std::endl;
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

    bool Parser::check(TokensTypes tk)
    {
        return current().type == tk;
    }

    ASTPtr Parser::ParseDeclarations()
    {
        switch (current().type)
        {

        case TokensTypes::TOKEN_IF:
            return ParseIfStatement();
        case TokensTypes::TOKEN_LOOP:
            return ParseLoop();
        case TokensTypes::TOKEN_PRINT:
            return ParsePrint();
        case TokensTypes::TOKEN_PRINT_ERROR:
            return ParsePrintE();
        case TokensTypes::TOKEN_PRINT_NEW_LINE:
            return ParsePrintNl();
        case TokensTypes::TOKEN_DEF:
            return ParseVarDeclaration();
        default:
            LogErrors::getInstance().addError("Invalid statement at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 2);
            LogErrors::getInstance().printErrors();
            throw Excepts::CompilationException("Invalid Statement");
        }
    }

    ASTPtr Parser::ParseDefinitions()
    {
        while (check(TokensTypes::TOKEN_ARROW_SET))
        {
            if (check(TokensTypes::TOKEN_ARROW_SET))
            {
                std::cout << "Is function" << std::endl;
            }
        }
        return std::make_shared<NilNode>();
    }

    ASTPtr Parser::ParseFuncDeclaration()
    {
        consume(TokensTypes::TOKEN_DEF);
        std::string var_name = consume(TokensTypes::TOKEN_IDENTIFIER).value;
        consume(TokensTypes::TOKEN_COLON);
        auto type = consume(current().type).type;
        consume(TokensTypes::TOKEN_FUNC);
        consume(TokensTypes::TOKEN_LPAREN);
        std::vector<ASTPtr> args;
        while (!check(TokensTypes::TOKEN_RPAREN))
        {
            args.push_back(ParseExpression(type));
        }
        consume(TokensTypes::TOKEN_RPAREN);
        consume(TokensTypes::TOKEN_ARROW_SET);
        consume(TokensTypes::TOKEN_LBRACKET);
        auto block = ParseBlock();
        consume(TokensTypes::TOKEN_RBRACKET);
        return std::make_shared<FunctionDefinitionNode>(var_name, args, block);
    }

    ASTPtr Parser::ParseIfStatement()
    {
        consume(TokensTypes::TOKEN_IF);        // consume the'if'
        consume(TokensTypes::TOKEN_LPAREN);    // consume the '(' parent
        auto condition = nullptr;              // conditional expression (e.g.: 1 == 2)
        consume(TokensTypes::TOKEN_RPAREN);    // consume ')'
        consume(TokensTypes::TOKEN_ARROW_SET); // -> consume the arrow set

        auto ifBranch = ParseDeclarations(); // if body

        ASTPtr butBranch = nullptr;
        ASTPtr butCondition = nullptr;
        if (check(TokensTypes::TOKEN_BUT))
        {
            consume(TokensTypes::TOKEN_BUT); // consume the 'but'
            consume(TokensTypes::TOKEN_LPAREN);
            while (!check(TokensTypes::TOKEN_RPAREN))
            { // checa se há uma condição no 'but'

                butCondition = nullptr;
            }
            consume(TokensTypes::TOKEN_RPAREN);
            consume(TokensTypes::TOKEN_LBRACKET);
            while (!check(TokensTypes::TOKEN_RBRACKET))
            {
                butBranch = ParseDeclarations(); // 'but' body
            }
            consume(TokensTypes::TOKEN_RBRACKET);
        }

        auto node = std::make_shared<IfStatement>();
        node->ifCondition = condition;
        node->ifBranch = ifBranch;
        node->butBranch = butBranch;
        node->butCondition = butCondition;

        return node;
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
                node->val = '\0';
                return std::make_shared<NilNode>();
            }
            else
            {
                LogErrors::getInstance().addError("Concatenation needs a predominant value (like str, int or others types)", 3);
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
        consume(TokensTypes::TOKEN_DEF);
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

    ASTPtr Parser::ParseExpression(TokensTypes types)
    {
        //consume the value based on the type of the variable
        switch (types)
        {
        case TokensTypes::TOKEN_STR:
            return std::make_shared<LiteralNode>(consume(TokensTypes::TOKEN_STRING_LITERAL).value);
        case TokensTypes::TOKEN_INT:
        {
            try {
                return std::make_shared<IntNode>(std::stoi(consume(TokensTypes::TOKEN_INT).value));
            } catch (std::out_of_range e) {
                std::cerr << "[Error]: Current value out of range at line " << current().line << " column " << current().column << std::endl;
                throw std::out_of_range("Index out of range");
            }
        }
        case TokensTypes::TOKEN_FLOAT:
            if (check(TokensTypes::TOKEN_FLOAT)) {
                return std::make_shared<FloatNode>(std::stof(consume(TokensTypes::TOKEN_FLOAT).value));
            } else if (check(TokensTypes::TOKEN_DOUBLE)) {
                LogErrors::getInstance().addWarning("Using double as float at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 6);
                return std::make_shared<FloatNode>(std::stof(consume(TokensTypes::TOKEN_DOUBLE).value));
            }
            
        case TokensTypes::TOKEN_DOUBLE:
            if (check(TokensTypes::TOKEN_DOUBLE)) {
                return std::make_shared<DoubleNode>(std::stod(consume(TokensTypes::TOKEN_DOUBLE).value));
            } else if (check(TokensTypes::TOKEN_FLOAT)) {
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
            if (!check(TokensTypes::TOKEN_INT)) LogErrors::getInstance().addError("", 26);
            consume(TokensTypes::TOKEN_INT);
            return std::make_shared<ByteNode>(val);
        }
        case TokensTypes::TOKEN_LONG_INT:
            if (std::stoll(current().value) < -2147483648 || std::stoll(current().value) > 2147483647)
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
            std::cout << "Token: " << Tokens::tokenTypeToString(types) << std::endl;
            std::cerr << "[Error]: Invalid variable value type at line " << current().line << " column " << current().column << std::endl;
            throw Excepts::CompilationException("Invalid Variable Value Type");
        }
    }

    ASTPtr Parser::ParseLoopExpression()
    {

        if (current().type == TokensTypes::TOKEN_INT || current().type == TokensTypes::TOKEN_STR || current().type == TokensTypes::TOKEN_FLOAT || current().type == TokensTypes::TOKEN_DOUBLE)
        {
            LogErrors::getInstance().addError("[Error]: loop keyword only accepts two types of argumments", 59);
            throw std::logic_error("Logical Error");
        }
        else if (check(TokensTypes::TOKEN_IDENTIFIER))
        {
            return std::make_shared<VariableNode>(consume(TokensTypes::TOKEN_IDENTIFIER).value);
            // consome o : para em seguida consumir o tipo da variavel
            consume(TokensTypes::TOKEN_COLON);
            // switch para verificação de tipos da variavel
            switch (current().type)
            {
            case TokensTypes::TOKEN_INT:
                return std::make_shared<IntNode>(std::stoi(consume(current().type).value));
            case TokensTypes::TOKEN_DOUBLE:
                return std::make_shared<DoubleNode>(std::stod(consume(current().type).value));
            case TokensTypes::TOKEN_FLOAT:
                return std::make_shared<FloatNode>(std::stof(consume(current().type).value));
            case TokensTypes::TOKEN_STR:
                return std::make_shared<LiteralNode>(consume(current().type).value);
            default:
                std::cerr << "[Error]: Unexpected variable type at Line " << current().line << " Column " << current().column << std::endl;
                throw std::runtime_error("Invalid type");
            }
        }
        else if (current().type == TokensTypes::TOKEN_TRUE)
        {
            consume(TokensTypes::TOKEN_TRUE);
            return std::make_shared<TrueOrFalseNode>(true);
        }
        else if (current().type == TokensTypes::TOKEN_FALSE)
        {
            consume(TokensTypes::TOKEN_FALSE);
            return std::make_shared<TrueOrFalseNode>(false);
        }
        else
        {
            std::cerr << "[Error]: Invalid type of expression" << std::endl;
            throw std::logic_error("Logic Error");
        }
    }

    ASTPtr Parser::ParseLoop()
    {
        auto node = std::make_shared<LoopNode>();
        consume(TokensTypes::TOKEN_LOOP);
        consume(TokensTypes::TOKEN_LPAREN);

        /// ParseLoopExpression will be check if have a varaible definition and his type or condition
        node->condition = ParseLoopExpression();

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