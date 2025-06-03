#include "log.h"
#include <iostream>

namespace Log
{
    void LogErrors::addError(const std::string& error, int exit_code)
    {
        logs.push_back("[Error]: " + error);
        this->code = exit_code;
    }

    void LogErrors::addWarning(const std::string &err, int exit) {
        warns.push_back("[Warning]: " + err);
        code = exit;
    }

    bool LogErrors::hasErrorsAndWarns()
    {
        return !logs.empty() || !warns.empty();
    }

    int LogErrors::exitCode() {
        if (logs.empty()) {
            return 0;
        }
        return code;
    }

    int LogErrors::getErrSize() {
        return this->logs.size();
    }

    int LogErrors::getWarnsSize() {
        return this->warns.size();
    }

    void LogErrors::printErrors()
    {
        for (int lo = 0; lo < logs.size(); lo++)
        {
            // writing errors and warnings to console
            std::cerr << logs[lo] << std::endl;
            std::cerr << warns[warns.size()] << std::endl;
        }
    }
}