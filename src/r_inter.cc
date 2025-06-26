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

#include "../src/includes/r_inter.hpp"
#include "../src/includes/ast.hpp"
#include "iostream"
#include "fstream"
#include "ctime"
#include "../src/includes/chunk.hpp"

namespace Rythin
{
    Interpreter::Interpreter(std::vector<ASTPtr> nodess)
    {
        this->nodes = nodess;
        for (auto &node : nodes)
        {
            Execute(node);
        }
    }

    void Interpreter::Execute(ASTPtr node)
    {
        auto pr = std::dynamic_pointer_cast<PrintNode>(node);
        if (pr)
        {
            printf(pr->val.c_str());
        }
        else
        {
            auto var = std::dynamic_pointer_cast<PrintE>(node);
            if (var)
            {
                std::cerr << var->val << std::endl;
            }
            else
            {
                auto var = std::dynamic_pointer_cast<PrintNl>(node);
                if (var)
                {
                    std::cout << var->val << std::endl;
                }
                else
                {
                    auto var = std::dynamic_pointer_cast<PrintErrorLog>(node);
                    if (var)
                    {
                    }
                }
            }
        }
    }

    void write_log(const std::string &message, const std::string &log_path = "/tmp/rythin.log")
    {
        static std::ofstream log_file(log_path, std::ios::out); // cria e sobrescreve
        log_file << message << std::endl;
    }
}