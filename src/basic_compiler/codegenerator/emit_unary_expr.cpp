// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_unary_expr.cpp
 * Purpose: Implement CodeGenerator::emitUnaryExpr (UnaryExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/Symbols.h"

using namespace gwbasic;

/*
 * Function: emitUnaryExpr
 * Summary: Emit IR for a unary expression.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - u: Parsed UnaryExpr node.
 * Returns:
 *  - std::string: SSA register name holding the resulting value.
 */
std::string CodeGenerator::emitUnaryExpr(std::ostringstream& out, const UnaryExpr* u) {
    auto inner = emitExpr(out, u->inner.get(), "");
    if (u->op == Symbols::PLUS.first()) return inner;
    if (u->op == Symbols::MINUS.first()) {
        std::string res = nextTemp();
        std::string ir = std::format("  {} = fsub double 0.0, {}", res, inner);
        out << ir << Symbols::LF;
        std::ostringstream m; m << "line " << currentLine_ << " UnaryExpr(-) -> " << ir; log() << m.str() << Symbols::LF;
        return res;
    }
    if (u->op == Symbols::EXCLAMATION.first()) {
        std::string isZero = nextTemp();
        { std::string ir = std::format("  {} = fcmp oeq double {}, 0.0", isZero, inner); out << ir << Symbols::LF; }
        std::string res = nextTemp();
        { std::string ir = std::format("  {} = uitofp i1 {} to double", res, isZero); out << ir << Symbols::LF; }
        return res;
    }
    return inner;
}
