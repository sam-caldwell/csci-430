// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_opt_expr.cpp
 * Purpose:
 *  - Define `AstOptimizer::optExpr`, the recursive expression simplifier
 *    that performs constant folding, unary simplifications, and algebraic
 *    identities.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/compiler/Metrics.h"
#include <memory>
#include <utility>

namespace gwbasic {

/**
 * Function: AstOptimizer::optExpr
 * Purpose:
 *  - Recursively rewrite an `Expr` to apply local simplifications and
 *    constant folding.
 * Inputs:
 *  - e: Owned expression to simplify (may be null)
 * Outputs:
 *  - Returns the (possibly replaced) simplified expression.
 * Details:
 *  - UnaryExpr: eliminates unary plus and folds unary minus for numbers.
 *  - BinaryExpr: folds arithmetic/comparisons; applies identities
 *    (x+0, x*1, x*0, x/1, etc.).
 */
// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size)
std::unique_ptr<Expr> AstOptimizer::optExpr(std::unique_ptr<Expr> expr) {
    if (expr == nullptr) {
        return expr;
    }
    if (auto* unary = dyn_cast<UnaryExpr>(expr.get())) {
        unary->inner = optExpr(std::move(unary->inner));
        if (unary->op == Symbols::PLUS.first()) {
            if (gMetrics != nullptr) {
                gMetrics->incUnaryElimPlus();
            }
            if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                return expr;
            }
            return std::move(unary->inner);
        }
        if (unary->op == Symbols::MINUS.first()) {
            if (double value = 0.0; asNumber(unary->inner.get(), value)) {
                if (gMetrics != nullptr) {
                    gMetrics->incUnaryConstMinus();
                }
                if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                    return expr;
                }
                return std::make_unique<NumberExpr>(-value);
            }
            return expr;
        }
        return expr;
    }
    if (auto* binary = dyn_cast<BinaryExpr>(expr.get())) {
        binary->lhs = optExpr(std::move(binary->lhs));
        binary->rhs = optExpr(std::move(binary->rhs));
        double left = 0.0;
        double right = 0.0;
        const bool leftIsNumber = asNumber(binary->lhs.get(), left);
        const bool rightIsNumber = asNumber(binary->rhs.get(), right);

        switch (binary->op) {
            case BinaryOp::Add:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldAdd();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left + right);
                }
                if (isZero(binary->lhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdAddZero();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::move(binary->rhs);
                }
                if (isZero(binary->rhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdAddZero();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::move(binary->lhs);
                }
                return expr;
            case BinaryOp::Sub:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldSub();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left - right);
                }
                if (isZero(binary->rhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdSubZero();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::move(binary->lhs);
                }
                return expr;
            case BinaryOp::Mul:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldMul();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left * right);
                }
                if (isZero(binary->lhs.get()) || isZero(binary->rhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdMulZero();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(0.0);
                }
                if (isOne(binary->lhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdMulOne();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::move(binary->rhs);
                }
                if (isOne(binary->rhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdMulOne();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::move(binary->lhs);
                }
                return expr;
            case BinaryOp::Div:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldDiv();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left / right);
                }
                if (isOne(binary->rhs.get())) {
                    if (gMetrics != nullptr) {
                        gMetrics->incIdDivOne();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::move(binary->lhs);
                }
                return expr;
            case BinaryOp::Eq:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldCmp();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left == right ? 1.0 : 0.0);
                }
                return expr;
            case BinaryOp::Ne:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldCmp();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left != right ? 1.0 : 0.0);
                }
                return expr;
            case BinaryOp::Lt:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldCmp();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left < right ? 1.0 : 0.0);
                }
                return expr;
            case BinaryOp::Le:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldCmp();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left <= right ? 1.0 : 0.0);
                }
                return expr;
            case BinaryOp::Gt:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldCmp();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left > right ? 1.0 : 0.0);
                }
                return expr;
            case BinaryOp::Ge:
                if (leftIsNumber && rightIsNumber) {
                    if (gMetrics != nullptr) {
                        gMetrics->incConstFoldCmp();
                    }
                    if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                        return expr;
                    }
                    return std::make_unique<NumberExpr>(left >= right ? 1.0 : 0.0);
                }
                return expr;
            default: return expr;
        }
    }
    return expr;
}
// NOLINTEND(readability-function-cognitive-complexity,readability-function-size)

} // namespace gwbasic
