#include <memory>
#include <vector>
// local
#include "r_parser.hpp"
#include "../includes/ast.hpp"
#include "../lexer/lex_types.hpp"
#include "../tokens/t_tokens.hpp"

using namespace std;

namespace Rythin
{
    vector<Tokens> nodes;
    int main()
    {
        // tests the variable definition
        {
            nodes.push_back(Tokens(TokensTypes::TOKEN_DEF, "def", 1, 3));
            nodes.push_back(Tokens(TokensTypes::TOKEN_IDENTIFIER, "var_name", 1, 5));
            nodes.push_back(Tokens(Tokens(TokensTypes::TOKEN_COLON, ":", 1, 7)));
            nodes.push_back(Tokens(TokensTypes::TOKEN_INT_32, "int32", 1, 9));
            nodes.push_back(Tokens(TokensTypes::TOKEN_ASSIGN, ":=", 1, 11));
            nodes.push_back(Tokens(TokensTypes::TOKEN_INT_32, "23", 1, 15));
            nodes.push_back(Tokens(TokensTypes::TOKEN_EOF, "\0", 2, 0));

            // parse it now
            Parser p(nodes);
            p.Parse();
        }

        return 0;
    }
}