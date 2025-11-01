// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include <sstream>

namespace gwbasic {

SemanticAnalyzer::ValueType SemanticAnalyzer::typeOf(const Expr* e) {
    if (!e) return ValueType::Number;
    if (dyn_cast<const NumberExpr>(e)) return ValueType::Number;
    if (dyn_cast<const StringExpr>(e)) return ValueType::String;
    if (dyn_cast<const CallExpr>(e)) return ValueType::Number;
    if (auto v = dyn_cast<const VarExpr>(e)) { (void)v; return ValueType::Number; }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        auto t = typeOf(u->inner.get());
        if (t == ValueType::String) {
            std::ostringstream m; m << "TypeError: unary '" << u->op << "' not applicable to string @ " << u->pos.line << ':' << u->pos.col; log(m.str());
            throw SemanticError(m.str());
        }
        return ValueType::Number;
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        auto lt = typeOf(b->lhs.get());
        auto rt = typeOf(b->rhs.get());
        switch (b->op) {
            case BinaryOp::Add:
            case BinaryOp::Sub:
            case BinaryOp::Mul:
            case BinaryOp::Div:
                if (lt == ValueType::String || rt == ValueType::String) {
                    std::ostringstream m; m << "TypeError: arithmetic on string @ " << e->pos.line << ':' << e->pos.col; log(m.str());
                    throw SemanticError(m.str());
                }
                return ValueType::Number;
            case BinaryOp::Eq:
            case BinaryOp::Ne:
            case BinaryOp::Lt:
            case BinaryOp::Le:
            case BinaryOp::Gt:
            case BinaryOp::Ge:
                if (lt == ValueType::String || rt == ValueType::String) {
                    if (lt != rt) {
                        std::ostringstream m; m << "TypeError: cannot compare string with number @ " << e->pos.line << ':' << e->pos.col; log(m.str());
                        throw SemanticError(m.str());
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

} // namespace gwbasic
