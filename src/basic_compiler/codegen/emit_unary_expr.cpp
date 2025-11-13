// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_unary_expr.cpp
 * Purpose: Implement CodeGenerator::emitUnaryExpr (UnaryExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include <format>
#include <sstream>
#include <string>

using namespace gwbasic;

/*
 * Function: emitUnaryExpr
 * Summary: Emit IR for a unary expression.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - unaryExpr: Parsed UnaryExpr node.
 * Returns:
 *  - std::string: SSA register name holding the resulting value.
 */
std::string CodeGenerator::emitUnaryExpr(std::ostringstream& out, const UnaryExpr* unary_expr) {
    const std::string inner = emitExpr(out, unary_expr->inner.get(), "");
    if (unary_expr->op == Symbols::PLUS.first()) {
        return inner;
    }
    if (unary_expr->op == Symbols::MINUS.first()) {
        const std::string res = nextTemp();
        out << std::format("  {} = fsub double 0.0, {}", res, inner) << Symbols::LF;
        return res;
    }
    if (unary_expr->op == Symbols::EXCLAMATION.first()) {
        const std::string isZero = nextTemp();
        const std::string res = nextTemp();
        out << std::format("  {} = fcmp oeq double {}, 0.0", isZero, inner) << Symbols::LF
            << std::format("  {} = uitofp i1 {} to double", res, isZero) << Symbols::LF;
        return res;
    }
    return inner;
}
