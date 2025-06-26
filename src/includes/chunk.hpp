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

#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <string>
#include <stdint.h>
#include <unordered_map>
#include "r_opcodes.hpp"

namespace Rythin
{
    typedef struct 
    {
        std::vector<int> line;
        uint8_t opcode;
        std::vector<std::string> constants;
        std::unordered_map<std::string, std::string> codes;
    } Chunk;

    // void addVariableCall(Chunk& ch, uint8_t op, std::string constant) {
    //     ch.opcode = op;
    //     ch.constants.push_back(constant);
    // }
}

#endif