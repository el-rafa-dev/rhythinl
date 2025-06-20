#include <vector>
#include <string.h>
#include <unordered_map>

// local include
#include "ast.h"
#include "ast_visit.h"

#ifndef SEMANTIC_VISITOR_HPP
#define SEMANTIC_VISITOR_HPP

namespace Semantic
{
    class SemanticAnalyses : public ASTVisitor
    {
    public:
        std::unordered_map<std::string, TokensTypes> symbolTable;
        TokensTypes lastExprType = TokensTypes::TOKEN_EOF;

        void visit(VariableDefinitionNode& node) override
        {
            // symbolTable[node.var_name, node.val] = node.var_name;
        }
    };

}

#endif