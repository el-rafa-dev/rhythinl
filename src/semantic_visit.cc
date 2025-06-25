#include "../src/includes/semantic_visitor.hpp"
#include <iostream>

namespace Rythin
{
    void SemanticAnalyzer::Visit(VariableDefinitionNode &node)
    {
        if (symbolTable.find(node.var_name) != symbolTable.end())
        {
            LogErrors::getInstance().addError("Variable name '" + node.var_name + "' already set!", 76, 0, 0);
            return;
        }

        for (auto &stb : symbolTable)
        {
            std::cout << "Var name: " << stb.first << " Type: " << Tokens::tokenTypeToString(stb.second) << std::endl;
            // check if the type value is compatible with type declared
            if (valTable.find(stb.first) != valTable.end())
            {
                // std::cout << "Type: " << Tokens::tokenTypeToString(stb.second) << "\n";

                // if (auto& value = std::dynamic_pointer_cast<LiteralNode>(&node.val))
                // {

                // }

            }
        }

        symbolTable.insert(std::make_pair(node.var_name, node.type));
        valTable.insert(std::make_pair(node.var_name, node.val));
        VisitNode(node.val);
    }

    void SemanticAnalyzer::Visit(VariableNode &node)
    {
        if (symbolTable.find(node.name) == symbolTable.end())
        {
            LogErrors::getInstance().addError("Variable not declared!", 67, 0, 0);
            return;
        }
    }

    void SemanticAnalyzer::Visit(BinOp &node)
    {
        VisitNode(node.left);
        VisitNode(node.right);
    }

    void SemanticAnalyzer::Visit(FunctionDefinitionNode &node)
    {
        // Em uma versão mais avançada, vou adicionar à symbol table e criar escopo
        symbolTable.insert(std::make_pair(node.var_name, node.type));
        VisitNode(node.block);
    }
}