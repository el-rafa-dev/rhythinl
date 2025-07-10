// Copyright (C) 2025 Rafael de Sousa (el-rafa-dev)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "../src/includes/log.hpp"
#include "../src/parser/r_parser.hpp"
#include <iostream>

#if defined(__linux__)
    #define WARNING "\x1b[1m\x1b[33m[Warning]:>\x1b[0m "
    #define ERROR "\x1b[1m\x1b[31m[Error]:>\x1b[0m "
#elif defined(_WIN32)
    #define WARNING "[Warning]:> "
    #define ERROR "[Error]:> "
#endif


namespace Log
{
    void LogErrors::addError(const std::string &error, int exit_code, int line, int column)
    {
        if (line == 0 && column == 0)
        {
            logs.push_back(ERROR + error);
        }
        else
        {
            logs.push_back(ERROR + error + " at line " + std::to_string(line) + " column " + std::to_string(column));
        }

        this->code = exit_code;
    }

    void LogErrors::addWarning(const std::string &err, int exit, int line, int column)
    {
        if (line == 0 && column == 0)
        {
            warns.push_back(WARNING + err);
        }
        else
        {
            warns.push_back(WARNING + err + " at line " + std::to_string(line) + " column " + std::to_string(column));
        }
        code = exit;
    }

    bool LogErrors::hasErrorsAndWarns()
    {
        return !logs.empty() || !warns.empty();
    }

    int LogErrors::exitCode()
    {
        if (logs.empty())
        {
            return 0;
        }
        return code;
    }

    int LogErrors::getErrSize()
    {
        return this->logs.size();
    }

    int LogErrors::getWarnsSize()
    {
        return this->warns.size();
    }

    void LogErrors::printAll()
    {
        printWarnings();
        printErrors();
    }

    void LogErrors::printWarnings()
    {
        for (int i = 0; i < warns.size(); i++)
        {
            std::cerr << warns[i] << std::endl;
        }
    }

    void LogErrors::printErrors()
    {
        for (int lo = 0; lo < logs.size(); lo++)
        {
            std::cerr << logs[lo] << std::endl;
        }
    }
}