#ifndef LOG_H
#define LOG_H

#include <stdexcept>
#include <vector>
#include <string>

class LogErrors : public std::exception
{
    static std::vector<std::string> logs;
    static int pos;
    static char cur_inp;
    static std::string input;

public:
    static void addError(std::string error)
    {
        logs.push_back(error);
    }

    static void addErrorInterpolated(std::string val, int line, int column)
    {
        cur_inp = (val.length() > 0) ? val[pos] : '\n';
        input = val;
        std::string inter_val;
        while (cur_inp != '"' && cur_inp != '\0')
        {
            if (cur_inp == '$')
            {
                advance_input(); // avanca o $
                if (cur_inp == '[')
                {
                    advance_input();
                    inter_val = cur_inp;
                }
            }
        }
    }

    static void advance_input()
    {
        pos++;
        cur_inp = (pos < input.length()) ? input[pos] : '\n';
    }

    static std::vector<std::string> getVector()
    {
        return logs;
    };
};

#endif