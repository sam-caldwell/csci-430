// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"

namespace gwbasic {

bool SemanticAnalyzer::isComparisonExpr(const Expr* e) const {
    if (auto b = dyn_cast<const BinaryExpr>(e)) {
        switch (b->op) {
            case BinaryOp::Eq:
            case BinaryOp::Ne:
            case BinaryOp::Lt:
            case BinaryOp::Le:
            case BinaryOp::Gt:
            case BinaryOp::Ge:
                return true;
            default:
                return false;
        }
    }
    return false;
}

} // namespace gwbasic
