# Copyright (C) 2025 Rafael de Sousa (el-rafa-dev)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.15...3.31 FATAL_ERROR)

project(Rhythin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

set(RHYTHIN_SRC_CORE
    src/rhythin.cc
    src/r_lex.cc
    src/r_parser.cc
    src/t_tokens.cc
    src/r_inter.cc
    src/log_errors.cc
    src/semantic_visit.cc
)

set(RHYTHIN_INCLUDES
    src/includes/ast_visit.hpp
    src/includes/ast.hpp
    src/includes/chunk.hpp
    src/includes/log.hpp
    src/includes/lex_types.hpp
    src/includes/r_inst.hpp
    src/includes/r_inter.hpp
    src/includes/r_lex.hpp
    src/includes/r_opcodes.hpp
    src/includes/r_parser.hpp
    src/includes/rexcept.hpp
    src/includes/semantic_visitor.hpp
    src/includes/t_tokens.hpp
    src/includes/val_types.hpp
)

# --- Creating the final executable ---
# The executable needs its own source files.
add_executable(rhythin ${RHYTHIN_SRC_CORE} ${RHYTHIN_INCLUDES})

if(NOT CMAKE_SYSTEM_NAME STREQUAL ${CMAKE_HOST_SYSTEM_NAME})
  message(WARNING "You are using a cache file of other OS! Clean the build first and re-run again!")
endif()

# add_executable(rhythin src/includes/chunk.hpp src/includes/rexcept.hpp src/includes/r_opcodes.hpp src/rhythin.cc src/includes/r_parser.hpp src/includes/r_inst.hpp src/includes/lex_types.hpp src/includes/r_lex.hpp src/r_lex.cc src/r_parser.cc src/includes/r_lex.hpp src/includes/t_tokens.hpp src/t_tokens.cc src/includes/ast.hpp src/includes/ast_visit.hpp src/includes/semantic_visitor.hpp src/semantic_v.cc src/r_inter.cc src/includes/r_inter.hpp src/includes/log.hpp src/log_errors.cc)