#include <stdexcept>
#include <vector>
#include <string>

#ifndef LOG_H
#define LOG_H

namespace log {
    class LogErrors : public std::exception {
        private:
        std::vector<std::string> logs;
        public:
        void addError(std::string error) {
            logs.push_back(error);
        }
        
        std::vector<std::string> getVector() {
            return this->logs;
        };
    };
}

#endif