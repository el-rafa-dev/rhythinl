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

#ifndef R_LEX_HPP
#define R_LEX_HPP

#include "t_tokens.hpp"
#include "lex_types.hpp"
#include <string>

namespace Rythin
{
    class Lexer
    {
    private:
        std::string code_input;
        char current_input;
        int position;
        int line;
        int column;

        void advance_tk(bool isComment = NULL);
        void skip_withspace();
        Tokens identifier();
        Tokens stringLiteral();
        char peekNextChar() const;
        std::string processInterpolation();
        std::string digits();

    public:
        Lexer(std::string input);
        Tokens next_tk();
    };
} // namespace Rythin

#endif // R_LEX_H