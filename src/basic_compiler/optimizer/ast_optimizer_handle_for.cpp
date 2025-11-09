// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_handle_for.cpp
 * Purpose: Implement AstOptimizer::optimizeForStmt handler.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/compiler/Metrics.h"
#include <memory>
#include <utility>
#include <vector>

using namespace gwbasic;

auto AstOptimizer::optimizeForStmt(std::unique_ptr<Stmt>& stmt,
                                   std::vector<std::unique_ptr<Stmt>>& out) -> bool {
    auto* const forStmt = dyn_cast<ForStmt>(stmt.get());
    if (forStmt == nullptr) {
        return false;
    }
    forStmt->start = optExpr(std::move(forStmt->start));
    forStmt->end   = optExpr(std::move(forStmt->end));
    if (forStmt->step) {
        forStmt->step = optExpr(std::move(forStmt->step));
    }
    if (forStmt->step && isOne(forStmt->step.get())) {
        if (gMetrics != nullptr) {
            gMetrics->incForStepElided();
        }
        if ((gMetrics == nullptr) || !gMetrics->isAnalyzeOnly()) {
            forStmt->step.reset();
        }
    }
    optimizeForBody(*forStmt);
    out.emplace_back(std::move(stmt));
    return true;
}
