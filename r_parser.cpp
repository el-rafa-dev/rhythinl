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
            LogErrors::getInstance().addError("Expected '" + Tokens::tokenTypeToString(tk) + "' but got: " + Tokens::tokenTypeToString(current().type) + "' at line " + std::to_string(current().line) + " column " + std::to_string(current().column), 4);
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
            LogErrors::getInstance().addError("Invalid statement at line" + std::to_string(current().line) + " column " + std::to_string(current().column), 2);
            throw Excepts::CompilationException("Invalid Statement");
        }
    }
    ASTPtr Parser::ParseIfStatement()
    {
        consume(TokensTypes::TOKEN_IF);        // consume the'if'
        consume(TokensTypes::TOKEN_LPAREN);    // consume the '(' parent
        auto condition = ParseExpression();    // conditional expression (e.g.: 1 == 2)
        consume(TokensTypes::TOKEN_RPAREN);    // consume ')'
        consume(TokensTypes::TOKEN_ARROW_SET); // -> consume the arrow set

        auto ifBranch = ParseDeclarations(); // if body

        ASTPtr butBranch = nullptr;
        ASTPtr butCondition = nullptr;
        if (check(TokensTypes::TOKEN_BUT))
        {
            consume(TokensTypes::TOKEN_BUT); // consume 'but'
            if (check(TokensTypes::TOKEN_LPAREN))
            { // checa se há uma condição no 'but'
                consume(TokensTypes::TOKEN_LPAREN);
                butCondition = ParseExpression();
                consume(TokensTypes::TOKEN_RPAREN);
            }
            butBranch = ParseDeclarations(); // 'but' body
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
            if (check(TokensTypes::TOKEN_STRING_LITERAL)) {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS)) {
                    consume(TokensTypes::TOKEN_PLUS);
                    node->val += consume(current().type).value;
                }
            } else if (check(TokensTypes::TOKEN_NIL)) {
                return std::make_shared<NilNode>();
            } else {
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
            if (check(TokensTypes::TOKEN_STRING_LITERAL)) {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS)) {
                    consume(TokensTypes::TOKEN_PLUS);
                    node->val += consume(current().type).value;
                }
            } else if (check(TokensTypes::TOKEN_NIL)) {
                return std::make_shared<NilNode>();
            } else {
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
            if (check(TokensTypes::TOKEN_STRING_LITERAL)) {
                node->val = consume(current().type).value;
                while (check(TokensTypes::TOKEN_PLUS)) {
                    consume(TokensTypes::TOKEN_PLUS);
                    node->val += consume(current().type).value;
                }
            } else if (check(TokensTypes::TOKEN_NIL)) {
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
        TokensTypes tk = consume(current().type).type;
        consume(TokensTypes::TOKEN_ASSIGN); // consome o '=' para pegar o valor ou expressão
        auto val = ParseExpression();
        return std::make_shared<VariableDefinitionNode>(name, tk, val);
    }

    ASTPtr Parser::ParseExpression()
    {
        switch (current().type)
        {
        case TokensTypes::TOKEN_STRING_LITERAL:
            return std::make_shared<LiteralNode>(consume(current().type).value);
        case TokensTypes::TOKEN_INT:
            return std::make_shared<IntNode>(std::stoi(consume(current().type).value));
        case TokensTypes::TOKEN_FLOAT:
            return std::make_shared<FloatNode>(std::stof(consume(current().type).value));
        case TokensTypes::TOKEN_DOUBLE:
            return std::make_shared<DoubleNode>(std::stod(consume(current().type).value));
        case TokensTypes::TOKEN_BYTES:
        {
            auto unsign_char = new unsigned char[current().value.length()];
            for (int i = 0; i < current().value.length(); i++)
            {
                unsign_char[i] = static_cast<unsigned char>(current().value[1]);
            }
            unsign_char[current().value.length()] = '\0';
            consume(current().type);
            delete[] unsign_char;
        }
        case TokensTypes::TOKEN_NIL:
            consume(TokensTypes::TOKEN_NIL);
            return std::make_shared<NilNode>();
        case TokensTypes::TOKEN_OBJECT:
            if (check(TokensTypes::TOKEN_INT)) {
                auto ptr = std::make_shared<IntNode>(std::stoi(consume(current().type).value));
                return std::make_shared<ObjectNode>(ptr);
            } else if (check(TokensTypes::TOKEN_DOUBLE)) {
                auto ptr = std::make_shared<DoubleNode>(std::stod(consume(current().type).value));
                return std::make_shared<ObjectNode>(ptr);
            } else if (check(TokensTypes::TOKEN_STR)) {
                auto ptr = std::make_shared<LiteralNode>(consume(current().type).value);
                return std::make_shared<ObjectNode>(ptr);
            } else if (check(TokensTypes::TOKEN_FLOAT)) {
                auto ptr = std::make_shared<FloatNode>(std::stof(consume(current().type).value));
                return std::make_shared<ObjectNode>(ptr);
            }
        default:
            std::cerr << "[Error]: Invalid variable type at line " << current().line << " column " << current().column << std::endl;
            // throw Excepts::CompilationException("Invalid Variable Type");
        }
    }

    ASTPtr Parser::ParseLoopExpression()
    {

        if (current().type == TokensTypes::TOKEN_INT || current().type == TokensTypes::TOKEN_STR || current().type == TokensTypes::TOKEN_FLOAT || current().type == TokensTypes::TOKEN_DOUBLE)
        {
            std::cerr << "[Error]: loop keyword only accepts two types of argumments" << std::endl;
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
                // throw std::runtime_error("Invalid type");
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
            // throw std::logic_error("Logic Error");
        }
    }

    /*ASTPtr Parser::ParsePrimary()
    {
        switch (current().type)
        {
        case TokensTypes::TOKEN_INT:
            return std::make_shared<IntNode>(std::stoi(consume(TokensTypes::TOKEN_INT).value));
        case TokensTypes::TOKEN_DOUBLE:
            return std::make_shared<DoubleNode>(std::stod(consume(TokensTypes::TOKEN_DOUBLE).value));
        case TokensTypes::TOKEN_FLOAT:
            return std::make_shared<FloatNode>(std::stof(consume(TokensTypes::TOKEN_FLOAT).value));
        case TokensTypes::TOKEN_IDENTIFIER:
            return std::make_shared<VariableNode>(consume(TokensTypes::TOKEN_IDENTIFIER).value);
        case TokensTypes::TOKEN_TRUE:
            consume(TokensTypes::TOKEN_TRUE);
            return std::make_shared<TrueOrFalseNode>(true);
        case TokensTypes::TOKEN_FALSE:
            consume(TokensTypes::TOKEN_FALSE);
            return std::make_shared<TrueOrFalseNode>(false);
        case TokensTypes::TOKEN_LPAREN:
            consume(TokensTypes::TOKEN_LPAREN);
            return ParseExpression(); // vai parsear a expressão entre os parenteses
            consume(TokensTypes::TOKEN_RPAREN);
        default:
            std::cerr << "[Error]: Invalid expression at line " << current().line << " Column: " << current().column << std::endl;
            throw Excepts::CompilationException("Invalid Expression");
        }
    }*/

    int Parser::GetPrecedence(TokensTypes tk)
    {
        switch (tk)
        {
        case TokensTypes::TOKEN_EQUAL:
        case TokensTypes::TOKEN_NOT_EQUAL:
            return 10;
        case TokensTypes::TOKEN_GREATER_THAN:
        case TokensTypes::TOKEN_GREATER_EQUAL:
        case TokensTypes::TOKEN_LESS_THAN:
        case TokensTypes::TOKEN_LESS_EQUAL:
            return 20;
        case TokensTypes::TOKEN_PLUS:
        case TokensTypes::TOKEN_MINUS:
            return 30;
        case TokensTypes::TOKEN_MULTIPLY:
        case TokensTypes::TOKEN_DIVIDE:
        case TokensTypes::TOKEN_MODULO:
            return 40;
        default:
            return -1;
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