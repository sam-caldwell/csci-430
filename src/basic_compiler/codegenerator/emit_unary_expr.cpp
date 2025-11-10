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
        out << std::format("  {} = fsub double 0.0, {}", res, inner) << Symbols::LF;
        return res;
    }
    if (u->op == Symbols::EXCLAMATION.first()) {
        std::string isZero = nextTemp();
        std::string res = nextTemp();
        out << std::format("  {} = fcmp oeq double {}, 0.0", isZero, inner) << Symbols::LF
            << std::format("  {} = uitofp i1 {} to double", res, isZero) << Symbols::LF;
        return res;
    }
    return inner;
}
