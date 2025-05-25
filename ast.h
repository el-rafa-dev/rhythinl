/***nesse header tem todos os Nós da ASTNode que é classe principal onde todo os nos herdam dela***/


#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "iostream"

namespace Rythin
{

    class ASTNode
    {
    public:
        virtual ~ASTNode() = default;
    };
    using ASTPtr = std::shared_ptr<ASTNode>;

    struct PrintNode : public ASTNode
    {
        std::string val;
    };

    struct PrintE : public ASTNode
    {
        std::string val;
    };

    struct PrintNl : public ASTNode
    {
        std::string val;
    };

    struct PrintErrorLog : public ASTNode
    {
        std::string val;
    };

    struct UsingNode : public ASTNode
    {
        std::string using_name, using_src;
        std::string from_name, from_src, get_var_name;
    };

    struct BinOp : public ASTNode
    {
        std::string op;
        ASTPtr left;
        ASTPtr right;
    };

    struct IfStatement : public ASTNode
    {
        ASTPtr ifCondition;
        ASTPtr ifBranch;
        ASTPtr butBranch;
        ASTPtr butCondition;
    };

    struct VariableNode : public ASTNode
    {
        std::string val;
        VariableNode(const std::string val) : val(val){}
    };

    struct VariableDefinitionNode : public ASTNode {
        std::string var_name;
        ASTPtr type;
        ASTPtr val;
        VariableDefinitionNode(const std::string var, ASTPtr type, ASTPtr val) : var_name(var), type(type), val(val) {}
    };

    struct LiteralNode : public ASTNode
    {
        std::string val;
        LiteralNode(std::string val) : val(val) {
            std::cout << "Literal node: " << val << std::endl;
        }
    };

    struct BlockNode : public ASTNode
    {
        std::vector<ASTPtr> statements;
    };

    struct LoopNode : public ASTNode
    {
        ASTPtr condition;
        ASTPtr body;
    };

    struct IntNode : public ASTNode {
        int val;
        IntNode(int v) : val(v) {}
    };

    struct DoubleNode : public ASTNode {
        double val;
        DoubleNode(double val) : val(val) {}
    };

    struct FloatNode : public ASTNode {
        float val;
        FloatNode(float val) : val(val) {}
    };

    struct TrueOrFalseNode : public ASTNode {
        bool val;
        TrueOrFalseNode(bool val) : val(val) {}
    };
}

#endif