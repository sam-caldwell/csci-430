// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_optimize_line_statements.cpp
 * Purpose: Implement AstOptimizer::optimizeLineStatements to keep
 *          AstOptimizer::optimize simple and shallow.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

auto AstOptimizer::optimizeLineStatements(std::vector<std::unique_ptr<Stmt>>& statements) -> void {
    std::vector<std::unique_ptr<Stmt>> out;
    out.reserve(statements.size());
    for (auto& st : statements) {
        if (optimizeAssignStmt(st, out)) { continue; }
        if (optimizePrintStmt(st, out)) { continue; }
        if (dyn_cast<IfStmt>(st.get())) {
            rewriteIf(st, out);
            continue;
        }
        if (optimizeForStmt(st, out)) { continue; }
        // Other statements unchanged
        out.emplace_back(std::move(st));
    }
    statements = std::move(out);
}
