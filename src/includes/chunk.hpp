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