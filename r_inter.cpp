#include "r_inter.h"
#include "ast.h"
#include "iostream"
#include "fstream"
#include "ctime"
#include "chunk.h"

namespace Rythin
{
    Interpreter::Interpreter(std::vector<ASTPtr> nodess)
    {
        this->nodes = nodess;
        for (auto &node : nodes)
        {
            Execute(node);
        }
    }

    void Interpreter::Execute(ASTPtr node)
    {
        auto pr = std::dynamic_pointer_cast<PrintNode>(node);
        if (pr)
        {
            printf(pr->val.c_str());
        }
        else
        {
            auto var = std::dynamic_pointer_cast<PrintE>(node);
            if (var)
            {
                std::cerr << var->val << std::endl;
            }
            else
            {
                auto var = std::dynamic_pointer_cast<PrintNl>(node);
                if (var)
                {
                    std::cout << var->val << std::endl;
                }
                else
                {
                    auto var = std::dynamic_pointer_cast<PrintErrorLog>(node);
                    if (var)
                    {
                    }
                }
            }
        }
    }

    void write_log(const std::string &message, const std::string &log_path = "/tmp/rythin.log")
    {
        static std::ofstream log_file(log_path, std::ios::out); // cria e sobrescreve
        log_file << message << std::endl;
    }
}