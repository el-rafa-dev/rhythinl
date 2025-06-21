#include "../src/includes/ast.h"
#include "../src/includes/semantic_visitor.hpp"

using namespace Rythin;

namespace Semantic {
        void SemanticAnalyses::visit(VariableDefinitionNode& node)
        {
            // symbolTable[node.var_name, node.val] = node.var_name;
            std::cout << "Value of variable: " << node.val <<"\n";
        }

        void SemanticAnalyses::visit(LoopConditionNode& node)
        {
            //nothing here por enquanto
        }

        void SemanticAnalyses::visit(LoopNode &node)
        {
        }

        void SemanticAnalyses::visit(FunctionDefinitionNode& node)
        {
        }
}