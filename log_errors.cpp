#include "log.h"
#include <iostream>

namespace Log
{
    void LogErrors::addError(const std::string& error, int exit_code)
    {
        logs.push_back("[Error]: " + error);
        this->code = exit_code;
    }

    bool LogErrors::hasErrors()
    {
        return !logs.empty();
    }

    int LogErrors::exitCode() {
        if (!logs.empty()) {
            return 0;
        }
        return code;
    }

    void LogErrors::printErrors()
    {
        for (int lo = 0; lo < logs.size(); lo++)
        {
            // writing errors to console
            std::cerr << logs[lo] << std::endl;
        }
    }
}