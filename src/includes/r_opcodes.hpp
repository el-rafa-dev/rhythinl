#ifndef R_OPCODES_HPP
#define R_OPCODES_HPP

#include <vector>
#include <variant>
#include <iostream>
#include <string>
#include <stdint.h>

using Value = std::variant<int, double, std::string>;

enum class OpCode : uint8_t {
    OP_ASSIGN,       //assignment [:=]
    OP_FIN,         //finish [fin FINISH_CODE (0 or a diferent number greeter or less than zero)]
    OP_RETURN,      //return
    OP_CONST,       //const var
    OP_BOOL,         //boolean
    OP_PRINT,       //print inline func
    OP_PRINT_E,     //print error func
    OP_PRINT_NL,    //print new line func
    OP_INT,         //int type
    OP_FLOAT,       //float type
    OP_FUNC,        //func definition type
    OP_DOUBLE,      //double type
    OP_STR,         //string type
    OP_SUB,         //minus -
    OP_MULTI,       //* multiplicaition
    OP_ADD,         //+ addition
    OP_DIV,         // [/] division
    OP_CONCAT,      // concatention
    OP_CALL,        // call
    OP_MOV,         //mov
    OP_LOAD_VAR,         //carrega a variavel na pilha da vm ou memoria virtual
    OP_STORE,       //store
    OP_JMP,          //jump
    OP_TRUE,         //true
    OP_FALSE,         //false
    OP_END,
    OP_NIL           //nil value or obj

};

#endif //R_OPCODES_H