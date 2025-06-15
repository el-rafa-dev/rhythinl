#include "../src/includes/log.h"
#include <iostream>

namespace Log
{
    void LogErrors::addError(const std::string& error, int exit_code)
    {
        logs.push_back("\x1b[1m\x1b[31m[Error]:>\x1b[0m " + error);
        this->code = exit_code;
    }

    void LogErrors::addWarning(const std::string &err, int exit) {
        warns.push_back("\x1b[1m\x1b[33m[Warning]:>\x1b[0m " + err);
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

    void LogErrors::printAll() {
        printWarnings();
        printErrors();
    }

    void LogErrors::printWarnings() {
        for (int i=0; i < warns.size(); i++) {
            std::cerr << warns[i] << std::endl;
        }
    }

    void LogErrors::printErrors()
    {
        for (int lo = 0; lo < logs.size(); lo++)
        {
            // writing errors and warnings to console
            std::cerr << logs[lo] << std::endl;
        }
    }
}