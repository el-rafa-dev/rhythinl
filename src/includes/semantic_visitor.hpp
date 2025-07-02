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
        std::unordered_map<std::string, TokensTypes> var_table;
        std::unordered_map<std::string, ASTPtr> var_values;
        std::unordered_map<std::string, TokensTypes> func_table;

    public:
        void Visit(VariableDefinitionNode &node) override;
        void Visit(VariableNode &node) override;
        void Visit(BinOp &node) override;
        void Visit(FunctionDefinitionNode &node) override;
        void Visit(InterpolationNode& node) override;
        // TODO: Adicionar outros Visit conforme eu for expandindo
    };
}

#endif