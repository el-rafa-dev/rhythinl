#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <unordered_map>
#include <string>

// Local Includes
#include "ast.hpp"
#include "ast_visit.hpp"
#include "t_tokens.hpp"
#include "log.hpp"

using namespace Log;

namespace Rythin
{
    class SemanticAnalyzer : public ASTVisitor
    {
    private:
        std::unordered_map<std::string, TokensTypes> symbolTable;
        std::unordered_map<std::string, ASTPtr> valTable;
        std::unordered_map<std::string, ASTPtr> func_argsTable;

    public:
        void Visit(VariableDefinitionNode &node) override;

        void Visit(VariableNode &node) override;

        void Visit(BinOp &node) override;

        void Visit(FunctionDefinitionNode &node) override;

        // TODO: Adicionar outros Visit conforme você for expandindo
    };
}

#endif