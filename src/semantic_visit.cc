#include "../src/includes/semantic_visitor.hpp"
#include <iostream>

// TODO: add more analyses for semantic analyzer!

namespace Rythin
{
    void SemanticAnalyzer::Visit(VariableDefinitionNode &node)
    {
        if (var_table.find(node.var_name) != var_table.end())
        {
            LogErrors::getInstance().addError("Variable name '" + node.var_name + "' already set!", 76, 0, 0);
            return;
        }

        for (auto &stb : var_table)
        {
            // check if the type value is compatible with type declared
            if (var_values.find(stb.first) != var_values.end())
            {
                // std::cout << "Type: " << Tokens::tokenTypeToString(stb.second) << "\n";

                // if (auto& value = std::dynamic_pointer_cast<LiteralNode>(&node.val))
                // {

                // }
            }
        }

        var_table.insert(std::make_pair(node.var_name, node.type));
        var_values.insert(std::make_pair(node.var_name, node.val));
        VisitNode(node.val);
    }

    void SemanticAnalyzer::Visit(VariableNode &node)
    {
        if (var_table.find(node.name) == var_table.end())
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
        if (var_table.find(node.var_name) != var_table.end())
        {
            LogErrors::getInstance().addError("The function name '" + node.var_name + "' already set and it's a " + Tokens::tokenTypeToString(node.type) + "!", 76, 0, 0);
            return;
        }
        // if (func_table.find(node.))
        func_table.insert(std::make_pair(node.var_name, node.type));
        VisitNode(node.block);
    }
}