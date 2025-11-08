// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/compiler/Metrics.h"

namespace gwbasic {

// Helpers are declared as private static methods on SemanticAnalyzer

/*
 * Function: SemanticAnalyzer::constEval
 * Inputs:
 *  - e: Expression to evaluate
 *  - out: Reference to receive constant value
 * Outputs:
 *  - bool: true if the expression evaluated to a constant number
 * Theory of operation:
 *  - Attempts constant folding for numbers, unary +/- and arithmetic/
 *    comparison binary operations recursively; returns false otherwise.
 */
bool SemanticAnalyzer::constEval(const Expr* e, double& out) {
    if (!e) return false;
    if (const auto n = dyn_cast<const NumberExpr>(e)) {
        out = n->value;
        return true;
    }
    if (const auto u = dyn_cast<const UnaryExpr>(e)) {
        return SemanticAnalyzer::constEvalUnary(*u, out);
    }
    if (const auto b = dyn_cast<const BinaryExpr>(e)) {
        return SemanticAnalyzer::constEvalBinary(*b, out);
    }
    return false;
}

} // namespace gwbasic
