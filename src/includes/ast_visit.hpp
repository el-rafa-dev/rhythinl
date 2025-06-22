#ifndef AST_VISIT_H
#define AST_VISIT_H

#include "ast.hpp"

class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(VariableDefinitionNode &node) = 0;
    virtual void visit(LoopConditionNode &node) = 0;
    virtual void visit(LoopNode &node) = 0;
    virtual void visit(FunctionDefinitionNode &node) = 0;
};

#endif