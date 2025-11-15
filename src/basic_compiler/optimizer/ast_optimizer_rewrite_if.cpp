// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_rewrite_if.cpp
 * Purpose: Implement AstOptimizer::rewriteIf (constant IF folding).
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/compiler/Metrics.h"
#include <memory>
#include <utility>
#include <vector>

using namespace gwbasic;

// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size)
auto AstOptimizer::rewriteIf(std::unique_ptr<Stmt>& statement,
                             std::vector<std::unique_ptr<Stmt>>& out) -> void {
    auto* ifStmt = dyn_cast<IfStmt>(statement.get());
    if (ifStmt == nullptr) {
        out.emplace_back(std::move(statement));
        return;
    }
    ifStmt->cond = optExpr(std::move(ifStmt->cond));
    if (double value = 0.0; asNumber(ifStmt->cond.get(), value)) {
        if (value != 0.0) {
            if (gMetrics != nullptr) {
                gMetrics->incIfConstTrueToGoto();
            }
            if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                out.emplace_back(std::move(statement));
            } else {
                auto gotoStmt = std::make_unique<GotoStmt>(ifStmt->targetLine);
                gotoStmt->pos = ifStmt->pos;
                out.emplace_back(std::move(gotoStmt));
            }
        } else {
            if (gMetrics != nullptr) {
                gMetrics->incIfConstFalseRemoved();
            }
            if (gMetrics != nullptr && gMetrics->isAnalyzeOnly()) {
                out.emplace_back(std::move(statement));
            } else {
                // Removed: append nothing
            }
        }
        return;
    }
    out.emplace_back(std::move(statement));
}
// NOLINTEND(readability-function-cognitive-complexity,readability-function-size)
