#include <vector>
#include <string.h>

//local includes/imports
#include "ast.h"

#ifndef SEMANTIC_VISITOR_HPP
#define SEMANTIC_VISITOR_HPP

class SemanticAnalyses
{
    public:
    void Analyse(std::vector<ASTPtr> nodes);
};


#endif