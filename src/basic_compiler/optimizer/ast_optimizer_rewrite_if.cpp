// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_rewrite_if.cpp
 * Purpose: Implement AstOptimizer::rewriteIf (constant IF folding).
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

auto AstOptimizer::rewriteIf(std::unique_ptr<Stmt>& st,
                             std::vector<std::unique_ptr<Stmt>>& out) -> void {
    auto* is = dyn_cast<IfStmt>(st.get());
    if (!is) {
        out.emplace_back(std::move(st));
        return;
    }
    is->cond = optExpr(std::move(is->cond));
    if (double v = 0.0; asNumber(is->cond.get(), v)) {
        if (v != 0.0) {
            if (gMetrics) gMetrics->incIfConstTrueToGoto();
            if (gMetrics && gMetrics->isAnalyzeOnly()) {
                out.emplace_back(std::move(st));
            } else {
                auto g = std::make_unique<GotoStmt>(is->targetLine);
                g->pos = is->pos;
                out.emplace_back(std::move(g));
            }
        } else {
            if (gMetrics) gMetrics->incIfConstFalseRemoved();
            if (gMetrics && gMetrics->isAnalyzeOnly()) {
                out.emplace_back(std::move(st));
            } else {
                // Removed: append nothing
            }
        }
        return;
    }
    out.emplace_back(std::move(st));
}

