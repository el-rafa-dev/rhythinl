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
        void addError(const std::string &error, int exit_code, int line, int column);
        void addWarning(const std::string &err, int exit, int line, int column);
        bool hasErrorsAndWarns();
        int exitCode();
        void printErrors();
        void printAll();
        void printWarnings();
    };
}

#endif