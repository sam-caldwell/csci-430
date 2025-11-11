// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_binary_expr.cpp
 * Purpose: Implement CodeGenerator::emitBinaryExpr (BinaryExpr lowering).
 */
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>
#include <string>

using namespace gwbasic;

/*
 * Function: emitBinaryExpr
 * Summary: Emit IR for a binary expression and return its result.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - b: Parsed BinaryExpr node.
 * Returns:
 *  - std::string: SSA register name holding the resulting value.
 */
std::string CodeGenerator::emitBinaryExpr(std::ostringstream& out, const BinaryExpr* binary_expr) {
    using enum gwbasic::BinaryOp;
    if (binary_expr->op == Eq || binary_expr->op == Ne || binary_expr->op == Lt || binary_expr->op == Le || binary_expr->op == Gt || binary_expr->op == Ge) {
        std::string pred_i1 = emitComparison(out, binary_expr);
        std::string pred_as_double = nextTemp();
        out << std::format("  {} = uitofp i1 {} to double", pred_as_double, pred_i1) << Symbols::LF;
        return pred_as_double;
    }
    auto lhs = emitExpr(out, binary_expr->lhs.get(), "");
    auto rhs = emitExpr(out, binary_expr->rhs.get(), "");
    std::string res = nextTemp();
    // Handle string concatenation specially for Add
    if (binary_expr->op == Add) {
        const bool lhsStr = isStringExpr(binary_expr->lhs.get());
        if (const bool rhsStr = isStringExpr(binary_expr->rhs.get()); lhsStr && rhsStr) {
            std::string lenL = nextTemp();
            std::string lenR = nextTemp();
            std::string total = nextTemp();
            std::string total1 = nextTemp();
            std::string buf = nextTemp();
            out << std::format("  {} = call i64 @strlen(ptr {})", lenL, lhs) << Symbols::LF
                << std::format("  {} = call i64 @strlen(ptr {})", lenR, rhs) << Symbols::LF
                << std::format("  {} = add i64 {}, {}", total, lenL, lenR) << Symbols::LF
                << std::format("  {} = add i64 {}, 1", total1, total) << Symbols::LF
                << std::format("  {} = call ptr @malloc(i64 {})", buf, total1) << Symbols::LF
                << std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, lhs) << Symbols::LF
                << std::format("  call ptr @strcat(ptr {}, ptr {})", buf, rhs) << Symbols::LF;
            return buf;
        }
    }

    // Map remaining ops to their IR mnemonics and emit once
    std::string op_mnemonic;
    switch (binary_expr->op) {
        case Add: op_mnemonic = "fadd"; break;
        case Sub: op_mnemonic = "fsub"; break;
        case Mul: op_mnemonic = "fmul"; break;
        case Div: op_mnemonic = "fdiv"; break;
        default: break;
    }
    if (!op_mnemonic.empty()) {
        out << std::format("  {} = {} double {}, {}", res, op_mnemonic, lhs, rhs) << Symbols::LF;
    }
    return res;
}
