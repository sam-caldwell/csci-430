// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_handle_for.cpp
 * Purpose: Implement AstOptimizer::optimizeForStmt handler.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

auto AstOptimizer::optimizeForStmt(std::unique_ptr<Stmt>& st,
                                   std::vector<std::unique_ptr<Stmt>>& out) -> bool {
    const auto fs = dyn_cast<ForStmt>(st.get());
    if (!fs) return false;
    fs->start = optExpr(std::move(fs->start));
    fs->end   = optExpr(std::move(fs->end));
    if (fs->step) fs->step = optExpr(std::move(fs->step));
    if (fs->step && isOne(fs->step.get())) {
        if (gMetrics) gMetrics->incForStepElided();
        if (!(gMetrics && gMetrics->isAnalyzeOnly())) fs->step.reset();
    }
    optimizeForBody(*fs);
    out.emplace_back(std::move(st));
    return true;
}

