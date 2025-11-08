// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_binary_expr.cpp
 * Purpose: Implement CodeGenerator::emitBinaryExpr (BinaryExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/BinaryExpr.h"

using namespace gwbasic;

std::string CodeGenerator::emitBinaryExpr(std::ostringstream& out, const BinaryExpr* b) {
    if (b->op == BinaryOp::Eq || b->op == BinaryOp::Ne || b->op == BinaryOp::Lt || b->op == BinaryOp::Le || b->op == BinaryOp::Gt || b->op == BinaryOp::Ge) {
        std::string i1 = emitComparison(out, b);
        std::string i1z = nextTemp();
        out << std::format("  {} = uitofp i1 {} to double", i1z, i1) << Symbols::LF;
        return i1z;
    }
    auto L = emitExpr(out, b->lhs.get(), "");
    auto R = emitExpr(out, b->rhs.get(), "");
    std::string res = nextTemp();
    switch (b->op) {
        case BinaryOp::Add: {
            const bool lhsStr = isStringExpr(b->lhs.get());
            if (const bool rhsStr = isStringExpr(b->rhs.get()); lhsStr && rhsStr) {
                std::string lenL = nextTemp();
                std::string lenR = nextTemp();
                std::string total = nextTemp();
                std::string total1 = nextTemp();
                std::string buf = nextTemp();
                out << std::format("  {} = call i64 @strlen(ptr {})", lenL, L) << Symbols::LF
                    << std::format("  {} = call i64 @strlen(ptr {})", lenR, R) << Symbols::LF
                    << std::format("  {} = add i64 {}, {}", total, lenL, lenR) << Symbols::LF
                    << std::format("  {} = add i64 {}, 1", total1, total) << Symbols::LF
                    << std::format("  {} = call ptr @malloc(i64 {})", buf, total1) << Symbols::LF
                    << std::format("  call ptr @strcpy(ptr {}, ptr {})", buf, L) << Symbols::LF
                    << std::format("  call ptr @strcat(ptr {}, ptr {})", buf, R) << Symbols::LF;
                return buf;
            }
            out << std::format("  {} = fadd double {}, {}", res, L, R) << Symbols::LF;
            break;
        }
        case BinaryOp::Sub: {
            out << std::format("  {} = fsub double {}, {}", res, L, R) << Symbols::LF;
            break;
        }
        case BinaryOp::Mul: {
            out << std::format("  {} = fmul double {}, {}", res, L, R) << Symbols::LF;
            break;
        }
        case BinaryOp::Div: {
            out << std::format("  {} = fdiv double {}, {}", res, L, R) << Symbols::LF;
            break;
        }
        default: break;
    }
    return res;
}

