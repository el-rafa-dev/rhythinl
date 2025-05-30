cmake_minimum_required(VERSION 3.31)
project(Rythin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(rythin chunk.h rexcept.h r_opcodes.h rhythin.cpp r_parser.h r_inst.h lex_types.h r_lex.h r_lex.cpp r_parser.cpp r_lex.h t_tokens.h t_tokens.cpp utils.cpp utils.h ast.h r_inter.cpp r_inter.h log.h log_errors.cpp)
