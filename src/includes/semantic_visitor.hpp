#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <unordered_map>
#include <string>

// Local Includes
#include "ast.hpp"
#include "ast_visit.hpp"
#include "t_tokens.hpp"
#include "log.hpp"

using namespace Log;

namespace Rythin
{
    class SemanticAnalyzer : public ASTVisitor
    {
    private:
        std::unordered_map<std::string, TokensTypes> symbolTable;
        std::unordered_map<std::string, ASTPtr> valTable;
        std::unordered_map<std::string, ASTPtr> func_argsTable;

    public:
        void Visit(VariableDefinitionNode& node) override
        {
            if (symbolTable.find(node.var_name) != symbolTable.end())
            {
                LogErrors::getInstance().addError("Variable name '" + node.var_name +"' already set!",76,0,0);
                return;
            }

            for (auto& stb : symbolTable)
            {
                std::cout << "Var name: " << stb.first << " Type: " << Tokens::tokenTypeToString(stb.second) << std::endl;
                // check if the type value is compatible with type declared
                if (valTable.find(stb.first) != valTable.end())
                {
                    // std::cout << "Type: " << Tokens::tokenTypeToString(stb.second) << "\n";
                    for (auto& vals : valTable)
                    {
                        
                        if (auto val = std::dynamic_pointer_cast<i32Node>(vals.second))
                        {
                            if (stb.second != TokensTypes::TOKEN_INT_32)
                            {
                                LogErrors::getInstance().addError("The value type for " + stb.first + " not matches with the type that you declared: " + Tokens::tokenTypeToString(stb.second), 99, 0 , 0);
                            }
                            std::cout << "int32 Value: " << val->val << std::endl;
                        }
                        if (auto val = std::dynamic_pointer_cast<LiteralNode>(vals.second))
                        {
                            if (stb.second != TokensTypes::TOKEN_STRING_LITERAL)
                            {
                                LogErrors::getInstance().addError("The value type for " + stb.first + " not matches with the type that you declared: " + Tokens::tokenTypeToString(stb.second), 99, 0 , 0);
                            }
                        }
                    }
                }
            }

            symbolTable[node.var_name] = node.type;
            valTable[node.var_name] = node.val;
            VisitNode(node.val);
        }

        void Visit(VariableNode& node) override
        {
            if (symbolTable.find(node.name) == symbolTable.end())
            {
                LogErrors::getInstance().addError("Variable not declared!", 67, 0, 0);
                return;
            }
        }

        void Visit(BinOp& node) override
        {
            VisitNode(node.left);
            VisitNode(node.right);
        }

        void Visit(FunctionDefinitionNode& node) override
        {
            // Em uma versão mais avançada, adicionar à symbol table e criar escopo
            VisitNode(node.block);
        }

        // TODO: Adicionar outros Visit conforme você for expandindo
    };
}

#endif