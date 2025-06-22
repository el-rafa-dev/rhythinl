#include <stdexcept>
#include <string>

#ifndef R_EXCEPTION_HPP
#define R_EXCEPTION_HPP

// customizeds exceptions
namespace Excepts
{
    class CompilationException : public std::exception
    {
    public:
        CompilationException(const std::string &msg) : msg(msg) {}

        const char *what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
    class SyntaxException : public std::exception
    {
    public:
        SyntaxException(const std::string &msg) : msg(msg) {}

        const char *what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}

#endif