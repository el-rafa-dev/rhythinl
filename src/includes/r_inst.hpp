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