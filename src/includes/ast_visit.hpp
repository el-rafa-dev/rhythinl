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
        virtual void Visit(PrintNode& node) {}
        virtual void Visit(PrintNl& node) {}
        virtual void Visit(VariableNode& node) {}
        virtual void Visit(LiteralNode& node) {}
        virtual void Visit(FunctionDefinitionNode& node) {}
        virtual void Visit(VariableDefinitionNode& node) {}
        virtual void Visit(BinOp& node) {}
        virtual void Visit(IfStatement& node) {}
        virtual void Visit(ReturnNode& node) {}
        virtual void Visit(BlockNode& node)
        {
            for (auto& stmt : node.statements)
                VisitNode(stmt);
        }

        // método genérico para despachar nós com base no tipo
        void VisitNode(ASTPtr node)
        {
            if (!node) return;

            if (auto n = std::dynamic_pointer_cast<PrintNode>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<PrintNl>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<VariableNode>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<LiteralNode>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<FunctionDefinitionNode>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<VariableDefinitionNode>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<BinOp>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<IfStatement>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<ReturnNode>(node)) Visit(*n);
            else if (auto n = std::dynamic_pointer_cast<BlockNode>(node)) Visit(*n);
            // e assim por diante para todos os nós que quiser suportar
        }
    };
}

#endif
