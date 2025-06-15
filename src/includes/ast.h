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
#include "t_tokens.h"
#include <iostream>

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
        std::string val; // the value is set by compiler/interpreter
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

    // the variable call node
    struct VariableNode : public ASTNode
    {
        std::string name;
        std::vector<ASTPtr> args;
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

    struct InterpolationNode : public ASTNode
    {
        std::string val;      // <-
        std::string var_name; // gets the var/function and return the value to the val
    };

    struct i32Node : public ASTNode
    {
        int32_t val;
        i32Node(int32_t val) : val(val) {}
    };

    struct i64Node : public ASTNode
    {
        int64_t val;
        i64Node(int64_t val) : val(val) {}
    };

    struct ByteNode : public ASTNode
    {
        unsigned char byte;
        ByteNode(unsigned char by) : byte(by) {}
    };

    struct f64Node : public ASTNode
    {
        // double is a float thats supports 8 bytes (or 64 bits)
        double val;
        f64Node(double val) : val(val) {}
    };

    struct f32Node : public ASTNode
    {
        float val;
        f32Node(float val) : val(val) {}
    };

    struct TrueOrFalseNode : public ASTNode
    {
        bool val;
        TrueOrFalseNode(bool val) : val(val) {}
    };

    struct IfStatement : public ASTNode
    {
        ASTPtr ifCondition;
        ASTPtr ifBranch;
        ASTPtr butBranch;
        ASTPtr butCondition;
        IfStatement(ASTPtr ifCondition, ASTPtr ifBranch, ASTPtr butBranch, ASTPtr butCondition) : ifCondition(ifCondition), ifBranch(ifBranch), butBranch(butBranch), butCondition(butCondition) {}
    };

    struct ObjectNode : public ASTNode
    {
        ASTPtr val;
        ObjectNode(ASTPtr val) : val(val) {}
    };

    struct ReturnNode : public ASTNode
    {
        ASTPtr val; // return value (pode ser um int, float/double, string, byte e etc)
        ReturnNode(ASTPtr val) : val(val) {}
    };

    struct FinishNode : public ASTNode
    {
        int val; // finish code is a intiger
        FinishNode(int val) : val(val) {}
    };

    struct NilNode : public ASTNode
    {
        NilNode() = default;
    };

    struct ExpressionNode : public ASTNode
    {
        std::string var_name;
        TokensTypes type;
    };

    struct BinOp : public ASTNode
    {
        TokensTypes op;     // operators
        ASTPtr left, right; // left value and right value
        BinOp(ASTPtr left, TokensTypes &op, ASTPtr right) : left(left), op(op), right(right)
        {
            if (auto var = std::dynamic_pointer_cast<i32Node>(left))
            {
                //std::cout << "Left value: " << var->val << std::endl;
                //std::cout << "Operator: " << Tokens::tokenTypeToString(op) << std::endl;
                if (auto var2 = std::dynamic_pointer_cast<i32Node>(right))
                {
                    //std::cout << "Right value: " << var2->val << std::endl;
                    switch (op)
                    {
                        case TokensTypes::TOKEN_PLUS:
                            std::cout << "Valor somado: " << var->val + var2->val << std::endl;
                            break;
                        case TokensTypes::TOKEN_MINUS:
                            std::cout << "Valor subtraido: " << var->val - var2->val << std::endl;
                            break;
                        case TokensTypes::TOKEN_MULTIPLY:
                            std::cout << "Valor multiplicado: " << var->val * var2->val << std::endl;
                            break;
                        case TokensTypes::TOKEN_DIVIDE:
                            std::cout << "Valor divido: " << var->val / var2->val << std::endl;
                            break;
                    }
                }
            }
        }
    };

    struct IfExpressionNode : public ASTNode
    {
        std::string var_name;      // name of the variable
        TokensTypes type;          // type of expression (== or other binary operators types)
        TokensTypes logic_divisor; // the divisor of the expressions like (&&, || and !)
        ASTPtr val;                // the value of the condition (like x > 2, the value of this expression is 2)
    };

    struct FunctionDefinitionNode : public ASTNode
    {
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