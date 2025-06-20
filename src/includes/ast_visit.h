#include "ast.h"

using namespace Rythin;

class ASTVisitor {
public:
    virtual void visit(VariableDefinitionNode& node) = 0;
    virtual void visit(LoopConditionNode& node) = 0;
    virtual void visit(FunctionDefinitionNode& node) = 0;
};