
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

#define EXIT_CODE 1

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
                Rythin::Parser parser(tokens, "rythin_file");
                std::vector<ASTPtr> nodes = parser.Parse();
                Interpreter interpreter(nodes);
            }
            else
            {

                std::cerr << "[Error]: Não foi possível abrir o arquivo." << std::endl;
            }
        }
    };

}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "-h") == 0)
    {
        std::cout << "[Help Arg.]\nUsage:\n\trythin [option] [file]\nExample:\n\tE.g.:rythin -f /home/user/rythin_code.ry\nArguments list:" << std::endl;
        std::cout << "\t[-f] [file-path/file-name] to execute a rythin file." << std::endl;
        std::cout << "\t[-h] to see this list." << std::endl;
    }
    else if (argc > 1 && strcmp(argv[1], "-f") == 0)
    {
        if (argv[2] != NULL)
        {
            Rythin::MainExecutor a;
            a.Run(argv[2]);
        }
        else
        {
            printf("A file must be specified to execute\n");
        }
    }
    else
    {
        printf("Invalid argument! See --help or -h to see the list of options!\n");
    }
    return 0;
}