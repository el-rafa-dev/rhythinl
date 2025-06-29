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

#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iostream>

/** * @name local includes 
 * @brief include files from the src dir 
 **/

#include "lex_types.hpp"
#include "t_tokens.hpp"
#include "rexcept.hpp"
#include "t_tokens.hpp"
// #include "ast_visit.hpp"


namespace Rythin
{
    class ASTVisitor;

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
    };

    struct IdentifierNode : public ASTNode
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
        LoopNode(std::string var_name, TokensTypes type, ASTPtr value, ASTPtr block) : var_name(var_name), type(type), value(value), block(block) {} // Added value to constructor
    };

    struct InterpolationNode : public ASTNode
    {
        std::string val;      // <- set the value of var/function name
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
        // double is a float thats supports 8 bytes (or 64 bits), one byte = 8 bits, 8x8 = 64
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
        // finish code is a intiger but if is a variable name defined or a variable function call (like var() ), is a ASTPtr
        int val;
        ASTPtr value;

        FinishNode(int val) : val(val) {}
        FinishNode(ASTPtr &value) : value(value) {}
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

    // Added: New AST node for unary operations (e.g., +val, -val)
    struct UnaryOp : public ASTNode
    {
        TokensTypes op; // The unary operator (e.g., TOKEN_PLUS, TOKEN_MINUS)
        ASTPtr operand; // The expression it operates on
        UnaryOp(TokensTypes op, ASTPtr operand) : op(op), operand(operand) {}
    };

    struct BinOp : public ASTNode
    {
        TokensTypes op;     // operators
        ASTPtr left, right; // left value and right value
        BinOp(ASTPtr left, TokensTypes &op, ASTPtr right) : left(left), op(op), right(right) {}
    };

    struct IfExpressionNode : public ASTNode
    {
        std::string var_name;      // name of the variable (can be empty if direct literal comparison)
        TokensTypes type;          // type of expression (== or other binary operators types, or TRUE/FALSE token type for direct bools)
        TokensTypes logic_divisor; // the divisor of the expressions like (&&, || and !)
        ASTPtr val;                // the value of the condition (like x > 2, the value of this expression is 2)
    };

    struct FunctionDefinitionNode : public ASTNode
    {
        std::string var_name;
        std::vector<ASTPtr> args;
        TokensTypes type;
        ASTPtr block;
        FunctionDefinitionNode(std::string name, TokensTypes tk, std::vector<ASTPtr> args, ASTPtr block) : var_name(name), type(tk), args(args), block(block) {
            // Debugging code removed for cleaner ASTNode.
            // This kind of debug output is usually handled by a separate ASTVisitor or interpreter.
            // while (auto test = std::dynamic_pointer_cast<PrintNl>(block))
            // {
            //     std::cout << "Value of printnl(): " << test->val << "\n";
            // }
        }
    };

    struct VariableDefinitionNode : public ASTNode
    {
        std::string var_name;
        TokensTypes type;
        ASTPtr val;
        VariableDefinitionNode(const std::string var, TokensTypes type, ASTPtr val) : var_name(var), type(type), val(val) {}
    };
}

#endif // AST_HPP
