#include "log.h"
#include <iostream>

namespace Log
{
    void LogErrors::addError(const std::string& error)
    {
        logs.push_back("[Error]: " + error);
    }

    bool LogErrors::hasErrors()
    {
        return !logs.empty();
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