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

#ifndef R_INTER_H
#define R_INTER_H

#include <vector>
#include <string>
#include "ast.hpp"
#include <filesystem>

namespace Rythin
{
    class Interpreter
    {
        public:
        std::filesystem::path path = std::filesystem::current_path();
        std::string getLogPath() {
            return path.string() + "/log";
        }
        time_t date = time(0);
        char formated_date[80];
        tm local_time = *localtime(&date);

        std::string log_path = "/tmp/rythin.log";

        std::string str_log_name;
        std::vector<ASTPtr> nodes;
        Interpreter(std::vector<ASTPtr> nodes);
        void Execute(ASTPtr node);
    };

}

#endif // R_INTER_H