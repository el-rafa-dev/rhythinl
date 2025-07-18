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

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "r_opcodes.hpp"

struct Instruction {
    OpCode opcode;
    int operand;
    bool hasOperand;
    std::string arg;

    Instruction(OpCode op) : opcode(op), operand(0), hasOperand(false) {}
    Instruction(OpCode op, std::string arg) : opcode(op), operand(0), hasOperand(false) {}
    Instruction(OpCode op, int val) : opcode(op), operand(val), hasOperand(true) {}
};

#endif //INSTRUCTION_H