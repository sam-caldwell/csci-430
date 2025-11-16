// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/semantics/SemanticError.h"
#include <cctype>
#include <cstddef>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::typeOf
 * Inputs:
 *  - e: Expression pointer to classify
 * Outputs:
 *  - ValueType: Number or String based on expression kind
 * Theory of operation:
 *  - Recursively inspects expression nodes, enforcing type rules for unary
 *    and binary operators, and throws on invalid string arithmetic or mixed
 *    string/number comparisons.
 */
// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size)
SemanticAnalyzer::ValueType SemanticAnalyzer::typeOf(const Expr* expr) {
    if (expr == nullptr) {
        return ValueType::Number;
    }
    if (isa<NumberExpr>(expr)) {
        return ValueType::Number;
    }
    if (isa<StringExpr>(expr)) {
        return ValueType::String;
    }
    if (const auto* call = dyn_cast<const CallExpr>(expr)) {
        // Array element reference has form A(i)
        if (arrays_.contains(call->callee)) {
            if (varNameIsString(call->callee)) {
                return ValueType::String;
            }
            return ValueType::Number;
        }
        // Built-in intrinsics: CHR$ returns string; ASC returns number.
        std::string funcNameUpper = call->callee;
        for (auto& chr : funcNameUpper) {
            chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
        }
        if (funcNameUpper == "CHR$") {
            return ValueType::String;
        }
        if (isKnownNumericFunction(funcNameUpper)) {
            return ValueType::Number;
        }
        if (isKnownStringFunction(funcNameUpper)) {
            return ValueType::String;
        }
        if (userFunctions_.contains(funcNameUpper)) {
            const DefFnStmt* def = userFunctions_.at(funcNameUpper);
            if (!def->fnName.empty() && def->fnName.back() == '$') {
                return ValueType::String;
            }
            return ValueType::Number;
        }
        // If function name ends with '$', treat as string (covers built-ins like CHR$)
        if (!call->callee.empty() && call->callee.back() == '$') {
            return ValueType::String;
        }
        return ValueType::Number;
    }
    if (const auto* var = dyn_cast<const VarExpr>(expr)) {
        // String if name has '$' suffix or falls under DEFSTR
        auto isStrName = [&](const std::string& name) -> bool {
            if (!name.empty() && name.back() == '$') {
                return true;
            }
            if (name.empty()) {
                return false;
            }
            const char firstChar = static_cast<char>(std::toupper(static_cast<unsigned char>(name[0])));
            if (firstChar < 'A' || firstChar > 'Z') {
                return false;
            }
            const DefaultKind defaultKind = defaultKinds_.at(static_cast<std::size_t>(firstChar - 'A'));
            return defaultKind == DefaultKind::Str;
        };
        if (isStrName(var->name)) {
            return ValueType::String;
        }
        return ValueType::Number;
    }
    if (const auto* unaryExpr = dyn_cast<const UnaryExpr>(expr)) {
        auto innerType = typeOf(unaryExpr->inner.get());
        if (innerType == ValueType::String) {
            std::ostringstream msg;
            msg << "TypeError: unary '" << unaryExpr->op
                << "' not applicable to string @ " << unaryExpr->pos.line << ':' << unaryExpr->pos.col;
            
            throw SemanticError(msg.str());
        }
        return ValueType::Number;
    }
    if (const auto* bin = dyn_cast<const BinaryExpr>(expr)) {
        auto leftType = typeOf(bin->lhs.get());
        auto rightType = typeOf(bin->rhs.get());
        switch (bin->op) {
            case BinaryOp::Add:
                if (leftType == ValueType::String || rightType == ValueType::String) {
                    if (leftType != ValueType::String || rightType != ValueType::String) {
                        std::ostringstream msg;
                        msg << "TypeError: cannot concatenate string with number @ "
                            << expr->pos.line << ':' << expr->pos.col;
                        
                        throw SemanticError(msg.str());
                    }
                    return ValueType::String;
                }
                [[fallthrough]];
            case BinaryOp::Sub:
            case BinaryOp::Mul:
            case BinaryOp::Div:
            case BinaryOp::IntDiv:
            case BinaryOp::Mod:
            case BinaryOp::Pow:
            case BinaryOp::And:
            case BinaryOp::Or:
                if (leftType == ValueType::String || rightType == ValueType::String) {
                    std::ostringstream msg;
                    msg << "TypeError: arithmetic on string @ "
                        << expr->pos.line << ':' << expr->pos.col;
                    
                    throw SemanticError(msg.str());
                }
                return ValueType::Number;
            case BinaryOp::Eq:
            case BinaryOp::Ne:
            case BinaryOp::Lt:
            case BinaryOp::Le:
            case BinaryOp::Gt:
            case BinaryOp::Ge:
                if (leftType == ValueType::String || rightType == ValueType::String) {
                    if (leftType != rightType) {
                        std::ostringstream msg;
                        msg << "TypeError: cannot compare string with number @ "
                            << expr->pos.line << ':' << expr->pos.col;
                        
                        throw SemanticError(msg.str());
                    }
                    return ValueType::Number;
                }
                return ValueType::Number;
            default:
                return ValueType::Number;
        }
    }
    return ValueType::Number;
}
// NOLINTEND(readability-function-cognitive-complexity,readability-function-size)

} // namespace gwbasic
