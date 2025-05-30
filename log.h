#ifndef LOG_H
#define LOG_H

#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>

namespace Log
{
    class LogErrors
    {
    private:
        LogErrors() = default;
        LogErrors(const LogErrors &) = delete;
        LogErrors &operator=(const LogErrors &) = delete;
        std::vector<std::string> logs;
    public:
        static LogErrors& getInstance() {
            static LogErrors instance;
            return instance;
        }
        void addError(const std::string &error);
        bool hasErrors();
        void printErrors();
    };
}

#endif