#ifndef LOG_H
#define LOG_H

#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>
#include <exception>

namespace Log
{
    class LogErrors : std::exception
    {
    private:
        LogErrors() = default;
        LogErrors(const LogErrors &) = delete;
        LogErrors &operator=(const LogErrors &) = delete;
        std::vector<std::string> logs;
        std::vector<std::string> warns;
        int code;
    public:
        static LogErrors& getInstance() {
            static LogErrors instance;
            return instance;
        }
        int getWarnsSize();
        int getErrSize();
        void addError(const std::string &error, int exit_code);
        void addWarning(const std::string &err, int exit);
        bool hasErrorsAndWarns();
        int exitCode();
        void printErrors();
        void printAll();
        void printWarnings();
    };
}

#endif