// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include <sstream>
#include <cctype>

namespace gwbasic {

void SemanticAnalyzer::analyzeExpr(const Expr* e) {
    if (!e) return;
    if (auto v = dyn_cast<const VarExpr>(e)) { reference(v->name, v->pos); return; }
    if (auto call = dyn_cast<const CallExpr>(e)) {
        // Array element reference: A(i[,j...]) or A$(...)
        if (arrays_.contains(call->callee)) {
            const auto& dims = arrays_.at(call->callee);
            if (call->args.size() != dims.size()) {
                std::ostringstream m; m << "ArityError: array '" << call->callee << "' expects " << dims.size() << " indices @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n';
                throw SemanticError(m.str());
            }
            for (const auto& a : call->args) {
                if (typeOf(a.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: array index must be numeric @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                analyzeExpr(a.get());
            }
            return;
        }
        std::string fn = call->callee; for (auto& ch : fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        // Built-in intrinsics
        const bool isBuiltinNum = isKnownNumericFunction(fn);
        const bool isBuiltinStr = isKnownStringFunction(fn);
        const bool isUser = (!isBuiltinNum && !isBuiltinStr) && userFunctions_.contains(fn);
        if (!isBuiltinNum && !isBuiltinStr && !isUser) {
            // Heuristic: in GW-BASIC, identifiers of the form A(1) are typically array references.
            // If it's not a builtin or user function and has arguments, treat as array use and
            // raise a clearer error if the array is not currently DIM'd.
            if (!call->args.empty()) {
                std::ostringstream m; m << "TypeError: array '" << call->callee << "' not DIM'd @ " << call->pos.line << ':' << call->pos.col;
                log() << m.str() << '\n';
                throw SemanticError(m.str());
            }
            std::ostringstream m; m << "Unknown function '" << call->callee << "' @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        if (isBuiltinStr) {
            if (fn == "CHR$") {
                if (call->args.size() != 1) { std::ostringstream m; m << "ArityError: function '" << call->callee << "' expects 1 arg @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            } else if (fn == "LEFT$" || fn == "RIGHT$") {
                if (call->args.size() != 2) { std::ostringstream m; m << "ArityError: function '" << call->callee << "' expects 2 args @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            } else if (fn == "MID$") {
                if (!(call->args.size() == 2 || call->args.size() == 3)) { std::ostringstream m; m << "ArityError: function '" << call->callee << "' expects 2 or 3 args @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            }
        } else if (isBuiltinNum) {
            if (fn == "SCREEN") {
                if (!(call->args.size() == 2 || call->args.size() == 3)) { std::ostringstream m; m << "ArityError: function 'SCREEN' expects 2 or 3 args @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            } else {
                int exp = expectedArity(fn);
                if (static_cast<int>(call->args.size()) != exp) { std::ostringstream m; m << "ArityError: function '" << call->callee << "' expects " << exp << " arg(s) @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            }
        } else {
            // user function: arity 1
            if (call->args.size() != 1) { std::ostringstream m; m << "ArityError: function '" << call->callee << "' expects 1 arg @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        }
        // Parameter type validation for user-defined function
        if (isUser && !call->args.empty()) {
            const DefFnStmt* def = userFunctions_[fn];
            const bool paramIsString = (!def->paramName.empty() && def->paramName.back() == '$');
            auto argT = typeOf(call->args[0].get());
            if (paramIsString && argT != ValueType::String) { std::ostringstream m; m << "TypeError: function '" << call->callee << "' expects string argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            if (!paramIsString && argT == ValueType::String) { std::ostringstream m; m << "TypeError: function '" << call->callee << "' expects numeric argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        }
        if (!call->args.empty()) {
            const Expr* a0 = call->args[0].get();
            if (isBuiltinNum && (fn == "LOG" || fn == "SQR" || fn == "SQRT") && isComparisonExpr(a0)) {
                std::ostringstream m; m << "DomainError: " << fn << " argument cannot be a comparison @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n';
                throw SemanticError(m.str());
            }
            double cval;
            if (isBuiltinNum && constEval(a0, cval)) {
                if ((fn == "SQR" || fn == "SQRT") && cval < 0.0) {
                    std::ostringstream m; m << "DomainError: SQR requires argument >= 0 @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n';
                    throw SemanticError(m.str());
                }
                if (fn == "LOG" && cval <= 0.0) {
                    std::ostringstream m; m << "DomainError: LOG requires argument > 0 @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n';
                    throw SemanticError(m.str());
                }
            }
        }
        // Warnings for unsafe numeric builtins
        if (isBuiltinNum) {
            if (fn == "PEEK") { log() << "Warning: unsafe PEEK at line " << call->pos.line << '\n'; }
            if (fn == "USR")  { log() << "Warning: unsafe USR at line "  << call->pos.line << '\n'; }
        }
        // Per-argument type checks
        if (isBuiltinStr) {
            if (fn == "CHR$") {
                if (typeOf(call->args[0].get()) == ValueType::String) { std::ostringstream m; m << "TypeError: function 'CHR$' requires numeric argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            } else if (fn == "LEFT$" || fn == "RIGHT$") {
                if (typeOf(call->args[0].get()) != ValueType::String) { std::ostringstream m; m << "TypeError: function '" << call->callee << "' expects string first argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                if (typeOf(call->args[1].get()) == ValueType::String) { std::ostringstream m; m << "TypeError: function '" << call->callee << "' expects numeric second argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            } else if (fn == "MID$") {
                if (typeOf(call->args[0].get()) != ValueType::String) { std::ostringstream m; m << "TypeError: function 'MID$' expects string first argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                if (typeOf(call->args[1].get()) == ValueType::String) { std::ostringstream m; m << "TypeError: function 'MID$' expects numeric second argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                if (call->args.size() == 3 && typeOf(call->args[2].get()) == ValueType::String) { std::ostringstream m; m << "TypeError: function 'MID$' expects numeric third argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            }
            for (const auto& arg : call->args) analyzeExpr(arg.get());
        } else if (isBuiltinNum) {
            for (const auto& arg : call->args) {
                if (fn == "ASC") {
                    if (typeOf(arg.get()) != ValueType::String) { std::ostringstream m; m << "TypeError: function 'ASC' expects string argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                } else {
                    if (typeOf(arg.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: function '" << call->callee << "' requires numeric argument @ " << call->pos.line << ':' << call->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                }
                analyzeExpr(arg.get());
            }
        } else {
            for (const auto& arg : call->args) analyzeExpr(arg.get());
        }
        return;
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) { analyzeExpr(b->lhs.get()); analyzeExpr(b->rhs.get()); return; }
    if (auto u = dyn_cast<const UnaryExpr>(e)) { analyzeExpr(u->inner.get()); return; }
    if (auto s = dyn_cast<const StringExpr>(e)) { strings_.insert(s->value); log() << "StringLiteral @ " << s->pos.line << ':' << s->pos.col << '\n'; return; }
}

} // namespace gwbasic
