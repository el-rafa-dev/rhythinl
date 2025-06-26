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

#include <stdexcept>
#include <string>

#ifndef R_EXCEPTION_HPP
#define R_EXCEPTION_HPP

// customizeds exceptions
namespace Excepts
{
    class CompilationException : public std::exception
    {
    public:
        CompilationException(const std::string &msg) : msg(msg) {}

        const char *what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
    class SyntaxException : public std::exception
    {
    public:
        SyntaxException(const std::string &msg) : msg(msg) {}

        const char *what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}

#endif