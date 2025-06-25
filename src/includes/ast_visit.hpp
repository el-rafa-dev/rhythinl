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
        }
    };
}

#endif
