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

#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "ast.hpp"
#include <unordered_map>
#include <string>

namespace Rythin
{
    class ASTVisitor
    {
    public:
        inline virtual void Visit(PrintNode& node) {};
        inline virtual void Visit(PrintNl& node) {}
        inline virtual void Visit(VariableNode& node) {}
        inline virtual void Visit(FunctionDefinitionNode& node) {}
        inline virtual void Visit(VariableDefinitionNode& node) {}
        inline virtual void Visit(BinOp& node) {}
        inline virtual void Visit(IfStatement& node) {}
        inline virtual void Visit(ReturnNode& node) {}
        inline virtual void Visit(InterpolationNode& node) {}
        inline virtual void Visit(BlockNode& node)
        {
            for (auto& stmt : node.statements)
                VisitNode(stmt);
        }

        // método genérico para despachar nós com base no tipo
        void VisitNode(ASTPtr node)
        {
            if (!node) return;

            if (auto n = std::dynamic_pointer_cast<PrintNode>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<PrintNl>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<VariableNode>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<FunctionDefinitionNode>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<VariableDefinitionNode>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<BinOp>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<IfStatement>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<ReturnNode>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<BlockNode>(node)) Visit(*n);
            if (auto n = std::dynamic_pointer_cast<InterpolationNode>(node)) Visit(*n);
        }
    };
}

#endif
