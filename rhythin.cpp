#include "vector"
#include "t_tokens.h"
#include "fstream"
#include "iostream"
#include "r_lex.h"
#include "r_inter.h"
#include "r_parser.h"
#include <cstring>
#include <ctype.h>
#include "r_opcodes.h"
#include "log.h"

using namespace Log;
std::fstream rythin_file;

namespace Rythin
{
    class MainExecutor
    {
    public:
        void Run(std::string file_name)
        {
            std::fstream rythin_file(file_name, std::ios::in); // Abre para leitura
            if (rythin_file.is_open())
            {
                std::string codigo_completo;
                std::string linha;
                while (std::getline(rythin_file, linha))
                {
                    if (!linha.empty())
                        codigo_completo += linha + '\n';
                }

                rythin_file.close();

                Lexer lexer(codigo_completo);

                std::vector<Tokens> tokens;

                while (true)
                {
                    Tokens token = lexer.next_tk();

                    tokens.push_back(token);

                    if (token.type == TokensTypes::TOKEN_EOF)
                        break;
                }
                Rythin::Parser parser(tokens, "rhythin_file");
                std::vector<ASTPtr> nodes = parser.Parse();
                Interpreter interpreter(nodes);
            }
            else
            {
                LogErrors::getInstance().addError("Não foi possível abrir o arquivo.", -2);
            }
        }
    };

}

void printHelp()
{
    std::cout << "[Help Arg.]\nUsage:\n\trhythin [option] [file]\nExample:\n\tE.g.:rhythin -f /home/user/rhythin_code.ry\nArguments list:" << std::endl;
    std::cout << "\t[-f] [--file] [file-path/file-name] to execute a rhythin file." << std::endl;
    std::cout << "\t[-h] [--help] to see this list." << std::endl;
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
            std::cerr << "[Bad compilation]: " << std::to_string(LogErrors::getInstance().getErrSize()) << " errors and " << std::to_string(LogErrors::getInstance().getWarnsSize()) << " warnings. But compiled. Exited with code: " << LogErrors::getInstance().exitCode() << std::endl;
            return LogErrors::getInstance().exitCode();
        }
        else
        {
            std::cout << "[Sucess]: Compiled without errors or warnings. Exit code: " << std::to_string(LogErrors::getInstance().exitCode()) << std::endl;
            return 0;
        }
    }
    else
    {
        LogErrors::getInstance().addError("A file must be specified to execute", 1);
        LogErrors::getInstance().printAll();
        return LogErrors::getInstance().exitCode();
    }
}

int main(int argc, char *argv[])
{
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
    } else if (argc > 1 && strcmp(argv[1], "--file") == 0) {
        return executeRun(argv);
    }
    else
    {
        LogErrors::getInstance().addError("Invalid argument! See --help or -h to see the list of options!", 6);
        LogErrors::getInstance().printAll();
        return LogErrors::getInstance().exitCode();
    }
    return 1;
}