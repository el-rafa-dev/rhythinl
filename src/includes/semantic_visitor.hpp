#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <unordered_map>
#include <string>

// Local Includes
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

    public:
        void Visit(VariableDefinitionNode& node) override
        {
            if (symbolTable.find(node.var_name) != symbolTable.end())
            {
                LogErrors::getInstance().addError("Variable name '" + node.var_name +"' already set!",76,0,0);
                return;
            }
            symbolTable[node.var_name] = node.type;
            VisitNode(node.val);
        }

        void Visit(VariableNode& node) override
        {
            if (symbolTable.find(node.name) == symbolTable.end())
            {
                std::cerr << "Erro: Variável '" << node.name << "' não declarada!" << std::endl;
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