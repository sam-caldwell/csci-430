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
SemanticAnalyzer::ValueType SemanticAnalyzer::typeOf(const Expr* e) {
    if (!e) return ValueType::Number;
    if (dyn_cast<const NumberExpr>(e)) return ValueType::Number;
    if (dyn_cast<const StringExpr>(e)) return ValueType::String;
    if (auto c = dyn_cast<const CallExpr>(e)) {
        // Built-in intrinsics: CHR$ returns string; ASC returns number.
        std::string fn = c->callee; for (auto &ch: fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (fn == "CHR$") return ValueType::String;
        if (isKnownNumericFunction(fn)) return ValueType::Number;
        if (isKnownStringFunction(fn)) return ValueType::String;
        if (userFunctions_.contains(fn)) {
            const DefFnStmt* def = userFunctions_.at(fn);
            if (!def->fnName.empty() && def->fnName.back() == '$') return ValueType::String;
            return ValueType::Number;
        }
        // If function name ends with '$', treat as string (covers built-ins like CHR$)
        if (!c->callee.empty() && c->callee.back() == '$') return ValueType::String;
        return ValueType::Number;
    }
    if (auto v = dyn_cast<const VarExpr>(e)) {
        // String if name has '$' suffix or falls under DEFSTR
        auto isStrName = [&](const std::string& nm) -> bool {
            if (!nm.empty() && nm.back() == '$') return true;
            if (nm.empty()) return false;
            char c0 = static_cast<char>(std::toupper(static_cast<unsigned char>(nm[0])));
            if (c0 < 'A' || c0 > 'Z') return false;
            DefaultKind dk = defaultKinds_[c0 - 'A'];
            return dk == DefaultKind::Str;
        };
        if (isStrName(v->name)) return ValueType::String;
        return ValueType::Number;
    }
    if (auto u = dyn_cast<const UnaryExpr>(e)) {
        auto t = typeOf(u->inner.get());
        if (t == ValueType::String) {
            std::ostringstream m; m << "TypeError: unary '" << u->op << "' not applicable to string @ " << u->pos.line << ':' << u->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        return ValueType::Number;
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        auto lt = typeOf(b->lhs.get());
        auto rt = typeOf(b->rhs.get());
        switch (b->op) {
            case BinaryOp::Add:
                if (lt == ValueType::String || rt == ValueType::String) {
                    if (lt != ValueType::String || rt != ValueType::String) {
                        std::ostringstream m; m << "TypeError: cannot concatenate string with number @ " << e->pos.line << ':' << e->pos.col; log() << m.str() << '\n';
                        throw SemanticError(m.str());
                    }
                    return ValueType::String;
                }
                [[fallthrough]];
            case BinaryOp::Sub:
            case BinaryOp::Mul:
            case BinaryOp::Div:
                if (lt == ValueType::String || rt == ValueType::String) {
                    std::ostringstream m; m << "TypeError: arithmetic on string @ " << e->pos.line << ':' << e->pos.col; log() << m.str() << '\n';
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
                        std::ostringstream m; m << "TypeError: cannot compare string with number @ " << e->pos.line << ':' << e->pos.col; log() << m.str() << '\n';
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
