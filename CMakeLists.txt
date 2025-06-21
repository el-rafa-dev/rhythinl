cmake_minimum_required(VERSION 3.31)
project(Rhythin/src)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(rhythin src/includes/chunk.h src/includes/rexcept.h src/includes/r_opcodes.h src/rhythin.cpp src/includes/r_parser.h src/includes/r_inst.h src/includes/lex_types.h src/includes/r_lex.h src/r_lex.cpp src/r_parser.cpp src/includes/r_lex.h src/includes/t_tokens.h src/t_tokens.cpp src/includes/ast.h src/includes/ast_visit.h src/includes/semantic_visitor.hpp src/semantic_v.cc src/r_inter.cpp src/includes/r_inter.h src/includes/log.h src/log_errors.cpp)
