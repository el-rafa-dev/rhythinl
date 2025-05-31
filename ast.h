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
        ASTPtr op;
        ASTPtr left;
        ASTPtr right;
        //BinOp(ASTPtr op, ASTPtr left, ASTPtr right) : op(op), left(left), right(right) {}
    };

    struct AddBinOp : public ASTNode { // + operator
        std::string op;
        AddBinOp(std::string op) : op(op) {}
    };

    struct DivBinOp : public ASTNode { // '/' operator
        std::string op;

        DivBinOp(std::string op) : op(op) {}
    };

    struct MinBinOp : public ASTNode { // '-' operator
        std::string op;
        MinBinOp(std::string op) : op(op) {}
    };

    struct MultBinOp : public ASTNode {
        std::string op;
        MultBinOp(std::string op) : op(op) {}
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

    struct LoopNode : public ASTNode
    {
        ASTPtr condition;
        ASTPtr body;
    };

    struct IntNode : public ASTNode
    {
        int val;
        IntNode(int v) : val(v) {}
    };

    struct ByteNode : public ASTNode
    {
        unsigned char* byte;
        ByteNode(unsigned char* by) : byte(by) {}
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

    struct VariableDefinitionNode : public ASTNode
    {
        std::string var_name;
        TokensTypes type;
        ASTPtr val;
        VariableDefinitionNode(const std::string var, TokensTypes type, ASTPtr val) : var_name(var), type(type), val(val) {
            if (auto var = std::dynamic_pointer_cast<LiteralNode>(val)) {
                if (type != TokensTypes::TOKEN_IDENTIFIER) {
                   std::cout << "Var Name: " << var_name << " Value: " << var->val << " Types: " << Tokens::tokenTypeToString(type) << std::endl;
                } else {
                    throw Excepts::CompilationException("Invalid type");
                }
            } else if (auto var = std::dynamic_pointer_cast<IntNode>(val)) {
                std::cout << "Var Name: " << var_name << " Value: " << var->val << " Types: " << Tokens::tokenTypeToString(type) << std::endl;
            } else if (auto var = std::dynamic_pointer_cast<DoubleNode>(val)) {
                std::cout << "Var Name: " << var_name << " Value: " << var->val << " Types: " << Tokens::tokenTypeToString(type) << std::endl;
            } else if (auto var = std::dynamic_pointer_cast<FloatNode>(val)) {
                std::cout << "Var Name: " << var_name << " Value: " << var->val << " Types: " << Tokens::tokenTypeToString(type) << std::endl;
            } else if (auto var = std::dynamic_pointer_cast<ByteNode>(val)) {
                std::cout << "Var Name: " << var_name << " Value: " << var->byte << " Types: " << Tokens::tokenTypeToString(type) << std::endl;
            } else if (auto var = std::dynamic_pointer_cast<ObjectNode>(val)) {
                std::cout << "Var Name: " << var_name << " Value: " << var << " Types: " << Tokens::tokenTypeToString(type) << std::endl;
            }
        }
    };
}

#endif