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
        std::string fn = call->callee; for (auto& ch : fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (!isKnownNumericFunction(fn)) {
            std::ostringstream m; m << "Unknown function '" << call->callee << "' @ " << call->pos.line << ':' << call->pos.col; log(m.str());
            throw SemanticError(m.str());
        }
        int exp = expectedArity(fn);
        if (static_cast<int>(call->args.size()) != exp) {
            std::ostringstream m; m << "ArityError: function '" << call->callee << "' expects " << exp << " arg(s) @ " << call->pos.line << ':' << call->pos.col; log(m.str());
            throw SemanticError(m.str());
        }
        if (!call->args.empty()) {
            const Expr* a0 = call->args[0].get();
            if ((fn == "LOG" || fn == "SQR" || fn == "SQRT") && isComparisonExpr(a0)) {
                std::ostringstream m; m << "DomainError: " << fn << " argument cannot be a comparison @ " << call->pos.line << ':' << call->pos.col; log(m.str());
                throw SemanticError(m.str());
            }
            double cval;
            if (constEval(a0, cval)) {
                if ((fn == "SQR" || fn == "SQRT") && cval < 0.0) {
                    std::ostringstream m; m << "DomainError: SQR requires argument >= 0 @ " << call->pos.line << ':' << call->pos.col; log(m.str());
                    throw SemanticError(m.str());
                }
                if (fn == "LOG" && cval <= 0.0) {
                    std::ostringstream m; m << "DomainError: LOG requires argument > 0 @ " << call->pos.line << ':' << call->pos.col; log(m.str());
                    throw SemanticError(m.str());
                }
            }
        }
        for (const auto& arg : call->args) {
            if (typeOf(arg.get()) == ValueType::String) {
                std::ostringstream m; m << "TypeError: function '" << call->callee << "' requires numeric argument @ " << call->pos.line << ':' << call->pos.col; log(m.str());
                throw SemanticError(m.str());
            }
            analyzeExpr(arg.get());
        }
        return;
    }
    if (auto b = dyn_cast<const BinaryExpr>(e)) { analyzeExpr(b->lhs.get()); analyzeExpr(b->rhs.get()); return; }
    if (auto u = dyn_cast<const UnaryExpr>(e)) { analyzeExpr(u->inner.get()); return; }
    if (auto s = dyn_cast<const StringExpr>(e)) { strings_.insert(s->value); std::ostringstream m; m << "StringLiteral @ " << s->pos.line << ':' << s->pos.col; log(m.str()); return; }
}

} // namespace gwbasic
