#include "../src/includes/log.h"
#include <iostream>

namespace Log
{
    void LogErrors::addError(const std::string& error, int exit_code, int line, int column)
    {
        if (line == 0 && column == 0)
        {
            logs.push_back("\x1b[1m\x1b[31m[Error]:>\x1b[0m " + error);
        } 
        else 
        {
            logs.push_back("\x1b[1m\x1b[31m[Error]:>\x1b[0m " + error + " at line " + std::to_string(line) + " column " + std::to_string(column));
        }
        this->code = exit_code;
    }

    void LogErrors::addWarning(const std::string &err, int exit, int line, int column) {
        if (line == 0 && column == 0)
        {
            warns.push_back("\x1b[1m\x1b[33m[Warning]:>\x1b[0m " + err );
        }
        else
        {
            logs.push_back("\x1b[1m\x1b[33m[Warning]:>\x1b[0m " + err + " at line " + std::to_string(line) + " column " + std::to_string(column)); 
        }
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