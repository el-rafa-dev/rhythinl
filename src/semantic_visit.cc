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

        var_table.insert(std::make_pair(node.var_name, node.type));
        var_values.insert(std::make_pair(node.var_name, node.val));
        VisitNode(node.val);
    }

    void SemanticAnalyzer::Visit(VariableNode &node)
    {
        if (var_table.find(node.name) == var_table.end())
        {
            LogErrors::getInstance().addError("Variable '" + node.name + "' not declared!", 67, 0, 0);
            return;
        }

        for (auto stmts : var_values) {
            if (auto var = std::dynamic_pointer_cast<TrueOrFalseNode>(stmts.second))
            {
                std::cout << "Name: " << stmts.first << " value: " << var->val << std::endl;
            }
            if (auto var = std::dynamic_pointer_cast<LiteralNode>(stmts.second)) 
            {
                std::cout << "Name: " << stmts.first << " value: " << var->val << std::endl;
            }
             if (auto var = std::dynamic_pointer_cast<InterpolationNode>(stmts.second)) 
            {
                std::cout << "Name: " << stmts.first << " Variable called: " << var->var_name << std::endl;

            }
            if (auto var = std::dynamic_pointer_cast<f32Node>(stmts.second)) 
            {
                std::cout << "Name: " << stmts.first << " Value: " << var->val << std::endl;

            }
            if (auto var = std::dynamic_pointer_cast<BinOp>(stmts.second)) 
            {
                // std::cout << "Name: " << stmts.first << " Left value: " << Tokens::tokenTypeToString(var->op) << std::endl;
                if (auto val_left = std::dynamic_pointer_cast<i32Node>(var->left))
                {
                    std::cout << "Value left: " << val_left->val << "\n";
                }
            }
        }
        
    }


    void SemanticAnalyzer::Visit(BinOp &node)
    {
        VisitNode(node.left);
        VisitNode(node.right);
        
    }

    void SemanticAnalyzer::Visit(FunctionDefinitionNode &node)
    {
        if (var_table.find(node.var_name) != var_table.end())
        {
            LogErrors::getInstance().addError("The name '" + node.var_name + "' already set and it's a " + Tokens::tokenTypeToString(node.type) + "!", 78, 0, 0);
            return;
        }

        var_table.insert(std::make_pair(node.var_name, node.type));
        func_table.insert(std::make_pair(node.var_name, node.type));
        VisitNode(node.block);
    }
}