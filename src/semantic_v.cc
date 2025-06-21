#include "../src/includes/ast.h"
#include "../src/includes/semantic_visitor.hpp"

namespace Semantic {
        void SemanticAnalyses::visit(VariableDefinitionNode& node)
        {
            // symbolTable[node.var_name, node.val] = node.var_name;
            std::cout << "Value of variable: " << node.val << std::endl;
        }

        void SemanticAnalyses::visit(Rythin::LoopConditionNode& node)
        {
            
        }

        void SemanticAnalyses::visit(Rythin::LoopNode &node)
        {
        }

        void SemanticAnalyses::visit(Rythin::FunctionDefinitionNode& node)
        {
        }
}