#include <vector>
#include <string.h>
#include <unordered_map>
#include <iostream>

// local include
#include "ast.h"
#include "ast_visit.h"

#ifndef SEMANTIC_VISITOR_HPP
#define SEMANTIC_VISITOR_HPP

using namespace Rythin;

namespace Semantic
{
    class SemanticAnalyses : public ASTVisitor
    {
    public:
        std::unordered_map<std::string, TokensTypes> symbolTable;
        TokensTypes lastExprType = TokensTypes::TOKEN_EOF;

        void visit(VariableDefinitionNode& node) override;

        void visit(LoopConditionNode& node) override;

        void visit(LoopNode &node) override;

        void visit(FunctionDefinitionNode& node) override;
    };

}

#endif