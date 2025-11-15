// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_optimize_line_statements.cpp
 * Purpose: Implement AstOptimizer::optimizeLineStatements to keep
 *          AstOptimizer::optimize simple and shallow.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <memory>
#include <utility>
#include <vector>

using namespace gwbasic;

auto AstOptimizer::optimizeLineStatements(std::vector<std::unique_ptr<Stmt>>& statements) -> void {
    std::vector<std::unique_ptr<Stmt>> out;
    out.reserve(statements.size());
    for (auto& statement : statements) {
        if (optimizeAssignStmt(statement, out)) {
            continue;
        }
        if (optimizePrintStmt(statement, out)) {
            continue;
        }
        if (isa<IfStmt>(statement.get())) {
            rewriteIf(statement, out);
            continue;
        }
        if (optimizeForStmt(statement, out)) {
            continue;
        }
        // Other statements unchanged
        out.emplace_back(std::move(statement));
    }
    statements = std::move(out);
}
