#ifndef R_INTER_H
#define R_INTER_H

#include <vector>
#include <string>
#include "ast.h"
#include <filesystem>

namespace Rythin
{
    class Interpreter
    {
        public:
        std::filesystem::path path = std::filesystem::current_path();
        std::string getLogPath() {
            return path.string() + "/log";
        }
        time_t date = time(0);
        char formated_date[80];
        tm local_time = *localtime(&date);

        std::string log_path = "/tmp/rythin.log";

        std::string str_log_name;
        std::vector<ASTPtr> nodes;
        Interpreter(std::vector<ASTPtr> nodes);
        void Execute(ASTPtr node);
    };

}

#endif // R_INTER_H