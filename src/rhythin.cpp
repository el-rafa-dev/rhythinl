#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>
// local includes
#include "../src/includes/t_tokens.h"
#include "../src/includes/r_lex.h"
#include "../src/includes/r_inter.h"
#include "../src/includes/r_parser.h"
#include "../src/includes/r_opcodes.h"
#include "../src/includes/log.h"
#include "../src/includes/semantic_visitor.hpp"

using namespace Log;
using namespace Semantic;
std::fstream file;

namespace Rythin
{
    class MainExecutor
    {
    public:
        void Run(std::string file_name)
        {
            // open to read of file
            std::fstream file(file_name, std::ios::in);
            if (file.is_open())
            {
                std::string code;
                std::string line;
                while (std::getline(file, line))
                {
                    if (!line.empty())
                        code += line + '\n';
                }

                file.close();

                Lexer lexer(code);

                std::vector<Tokens> tokens;

                while (true)
                {
                    Tokens token = lexer.next_tk();

                    tokens.push_back(token);

                    if (token.type == TokensTypes::TOKEN_EOF)
                        break;
                }
                Rythin::Parser parser(tokens);
                std::vector<ASTPtr> nodes = parser.Parse();

                SemanticAnalyses as;
                // start the semantic analyses here
                for (auto &stmts : nodes)
                {
                    stmts->accept(as);
                }

                // only for tests, a tree-walk interpreter
                Interpreter interpreter(nodes);
            }
            else
            {
                LogErrors::getInstance().addError("could not open the file", 5, 0, 0);
            }
        }
    };

}

void printHelp()
{
    std::cout << "[Help Arg.]\nUsage:\n\trhythin [option] [file]\nExample:\n\tE.g.:rhythin -f /home/user/rhythin_code.ry\nArguments list:" << std::endl;
    std::cout << "\t[-f] [--file] [file-path/file-name] to execute a rhythin file." << std::endl;
    std::cout << "\t[-h] [--help] to see this list." << std::endl;
    std::cout << "\t[-v] [--version] to see the version of the Rhythin" << std::endl;
}

int executeRun(char *argv[])
{
    if (argv[2] != NULL)
    {
        Rythin::MainExecutor a;
        a.Run(argv[2]);
        if (LogErrors::getInstance().hasErrorsAndWarns())
        {

            LogErrors::getInstance().printErrors();
            std::cerr << "\x1b[1m\x1b[31m[Bad compilation]:>\x1b[0m " << std::to_string(LogErrors::getInstance().getErrSize()) << " errors and " << std::to_string(LogErrors::getInstance().getWarnsSize()) << " warnings. Exited with code: " << LogErrors::getInstance().exitCode() << std::endl;
            return LogErrors::getInstance().exitCode();
        }
        else
        {
            std::cout << "\x1b[1m\x1b[32m[Sucess]:>\x1b[0m Compiled without errors or warnings. Exit code: " << std::to_string(LogErrors::getInstance().exitCode()) << std::endl;
            return 0;
        }
    }
    else
    {
        LogErrors::getInstance().addError("A file must be specified to execute", 1, 0, 0);
        LogErrors::getInstance().printAll();
        return LogErrors::getInstance().exitCode();
    }
}

void printVersion()
{
    std::cout << "\x1b[1m\x1b[31m               ...:::::^:::..                         \x1b[0m\x1b[1m\x1b[31m[Rhythin] :: [Version] :: [0.0.0.1-01]\x1b[0m      " << std::endl;
    std::cout << "\x1b[1m\x1b[31m             ...::^~7J5PGGBBBG57::.                   \x1b[0m\x1b[1m\x1b[31m[Author]  :: [Rafael de Sousa]\x1b[0m              " << std::endl;
    std::cout << "\x1b[1m\x1b[31m           .::^!?Y5B&@@@@@@@@@@@@B7::.                \x1b[0m\x1b[1m\x1b[31m[Copyright] (C) [2025] [Rafael de Sousa]\x1b[0m    " << std::endl;
    std::cout << "\x1b[1m\x1b[31m         .:^~75#&@@@@@@@@@@@@@@@@@@@Y::                                                                                        " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        .^^Y#@@@@@@@@&#G5Y?7!~J@@@@@&^:.                                                                                       " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        ^^J@@@@@@B5J7~:...... .?#@@@@@5..                                                                                      " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        ^^7&@@@@P::..    ..:7B@@@@@@Y...                                                                                       " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        .^^B@@@@J::    ..:?B@@@@@@#7..                                                                                         " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        .^^#@@@@7::.....:~Y#@@@@@@&Y:                                                                                          " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        :^~&@@@@!::  ::JG@@@@@@@#J:                                                                                            " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        :^!@@@@&!?G&@@@@@@&G!@@@.                                                                                              " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        :^7@@@@@&@@@@@@#P?^@@@.                                                                                                " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        ^^J@@@@@@@@@@@B7@@^:..                                                                                                 " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        ^^Y@@@@@@@@@@@@@&&BG5Y?7~^:..                                                                                          " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        ^^5@@@@@GB&@@@@@@@@@@@@@&@@@@@J!^:...                                                                                  " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        .^^P@@@@@!:^7JPB&@@@@@@@@@@@@@@@@@&#5!:.                                                                               " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        .^^P@@@@@5::...:^~7J5PGB##&@@@@@@@@@@#::.                                                                              " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        ^^7&@@@@PP:^.         ::^~!7?JJJJJ?!::                                                                                 " << std::endl;
    std::cout << "\x1b[1m\x1b[31m        .^^!J5PY@^^.                                                                                                           " << std::endl;
    std::cout << "\x1b[1m\x1b[31m          ^^@@@@^^·                                                                                                            " << std::endl;
    std::cout << "\x1b[1m\x1b[31m                                                                                                                        \x1b[0m" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 0 && argv == 0)
    {
        LogErrors::getInstance().addError("No argument specified. See --help or -h to see the list of options.", 6, 0, 0);
        LogErrors::getInstance().printAll();
        exit(LogErrors::getInstance().exitCode());
    }

    if (argc > 1 && strcmp(argv[1], "-h") == 0)
    {
        printHelp();
    }
    else if (strcmp(argv[1], "--help") == 0)
    {
        printHelp();
    }
    else if (argc > 1 && strcmp(argv[1], "-f") == 0)
    {
        return executeRun(argv);
    }
    else if (argc > 1 && strcmp(argv[1], "--file") == 0)
    {
        return executeRun(argv);
    }
    else if (argc > 1 && strcmp(argv[1], "-v") == 0)
    {
        printVersion();
    }
    else if (argc > 1 && strcmp(argv[1], "--version") == 0)
    {
        printVersion();
    }
    else
    {
        LogErrors::getInstance().addError("Invalid argument! See --help or -h to see the list of options!", 6, 0, 0);
        LogErrors::getInstance().printAll();
        return LogErrors::getInstance().exitCode();
    }
    return 1;
}