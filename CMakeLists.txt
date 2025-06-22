cmake_minimum_required(VERSION 3.15)

project(Rhythin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

set(RHYTHIN_SRC_CORE
    src/r_lex.cc
    src/r_parser.cc
    src/t_tokens.cc
    src/semantic_v.cc
    src/r_inter.cc
    src/log_errors.cc
)

#add_library(rhythin_core STATIC ${RHYTHIN_SRC_CORE})

set(rhythin_core
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
add_executable(rhythin src/rhythin.cc ${rhythin_core})

# target_link_libraries(rhythin PRIVATE rhythin_core)


# if (CMAKE_BUILD_TYPE STREQUAL "Release")
#     target_compile_definitions(rhythin_core PRIVATE RELEASE_MODE)
#     target_compile_definitions(rhythin PRIVATE RELEASE_MODE)
# endif()
#add_executable(rhythin src/includes/chunk.hpp src/includes/rexcept.hpp src/includes/r_opcodes.hpp src/rhythin.cc src/includes/r_parser.h src/includes/r_inst.h src/includes/lex_types.h src/includes/r_lex.h src/r_lex.cpp src/r_parser.cpp src/includes/r_lex.h src/includes/t_tokens.h src/t_tokens.cpp src/includes/ast.h src/includes/ast_visit.h src/includes/semantic_visitor.hpp src/semantic_v.cc src/r_inter.cpp src/includes/r_inter.h src/includes/log.h src/log_errors.cpp)