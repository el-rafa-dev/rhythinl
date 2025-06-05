/***nesse header tem todos os Nós da ASTNode que é classe principal onde todo os nos herdam dela***/

#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "iostream"
#include "lex_types.h"
#include "t_tokens.h"
#include "rexcept.h"

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

    struct CinputNode : public ASTNode
    {
        std::string val; //the value is set by compiler/interpreter
        std::string msg;
        int time;
        CinputNode() {}
        CinputNode(std::string msg) : msg(msg) {}
        CinputNode(std::string msg, int time) : msg(msg), time(time) {}
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
        TokensTypes op;  //operators
        ASTPtr left;
        ASTPtr right;
        BinOp(TokensTypes op, ASTPtr left, ASTPtr right) : op(op), left(left), right(right) {}
    };

    struct IfStatement : public ASTNode
    {
        ASTPtr ifCondition;
        ASTPtr ifBranch;
        ASTPtr butBranch;
        ASTPtr butCondition;
        IfStatement(ASTPtr ifCondition, ASTPtr ifBranch, ASTPtr butBranch, ASTPtr butCondition) : ifCondition(ifCondition), ifBranch(ifBranch), butBranch(butBranch), butCondition(butCondition) {}
    };

    struct VariableNode : public ASTNode
    {
        std::string val;
        VariableNode(const std::string val) : val(val) {}
    };

    struct LiteralNode : public ASTNode
    {
        std::string val;
        LiteralNode(std::string val) : val(val) {}
    };

    struct BlockNode : public ASTNode
    {
        std::vector<ASTPtr> statements;
    };

    struct LoopConditionNode : public ASTNode
    {
        ASTPtr condition;
        ASTPtr body;
    };

    struct LoopNode : public ASTNode
    {
        std::string var_name;
        TokensTypes type;
        ASTPtr value;
        ASTPtr block;
        LoopNode(std::string var_name, TokensTypes type, ASTPtr value, ASTPtr block) : var_name(var_name), type(type), block(block) {}
    };

    struct IntNode : public ASTNode
    {
        int val;
        IntNode(int v) : val(v) {}
    };

    struct InterpolationNode : public ASTNode 
    {
        std::string val;
        std::string var_name;
    };

    struct LIntNode : public ASTNode {
        long int val;
        LIntNode(int val) : val(val) {}
    };

    struct ByteNode : public ASTNode
    {
        unsigned char byte;
        ByteNode(unsigned char by) : byte(by) {}
    };

    struct DoubleNode : public ASTNode
    {
        double val;
        DoubleNode(double val) : val(val) {}
    };

    struct FloatNode : public ASTNode
    {
        float val;
        FloatNode(float val) : val(val) {}
    };

    struct TrueOrFalseNode : public ASTNode
    {
        bool val;
        TrueOrFalseNode(bool val) : val(val) {}
    };

    struct ObjectNode : public ASTNode {
        ASTPtr val;
        ObjectNode(ASTPtr val) : val(val) {}
    };

    struct ReturnNode : public ASTNode {
        ASTPtr val; //return value (pode ser um int, float/double, string ou byte)
        ReturnNode(ASTPtr val) : val(val) {}
    };

    struct FinishNode : public ASTNode {
        int val; //finish code is a intiger
        FinishNode(int val) : val(val) {}
    };

    struct NilNode : public ASTNode {
        NilNode() = default;
    };

    struct ExpressionNode : public ASTNode
    {
        std::string var_name;
        TokensTypes type;
        
    };

    struct IfExpressionNode : public ASTNode
    {
        std::string& var_name; //name of the variable
        TokensTypes type;   //type of expression (== or other types)
        TokensTypes logic_divisor; // the divisor of the expressions like (&&, || and !)
        ASTPtr val; //the value of the condition (like x > 2, the value of this expression is 2)
        IfExpressionNode(std::string& var_name, TokensTypes type, TokensTypes logic_divisor, ASTPtr val) : var_name(var_name), type(type), logic_divisor(logic_divisor), val(val) {}
    };

    struct FunctionDefinitionNode : public ASTNode {
        std::string var_name;
        std::vector<ASTPtr> args;
        ASTPtr block;
        FunctionDefinitionNode(std::string name, std::vector<ASTPtr> args, ASTPtr block) : var_name(name), args(args), block(block) {}
    };

    struct VariableDefinitionNode : public ASTNode
    {
        std::string var_name;
        TokensTypes type;
        ASTPtr val;
        VariableDefinitionNode(const std::string var, TokensTypes type, ASTPtr val) : var_name(var), type(type), val(val) {}
    };
}

#endif