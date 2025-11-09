// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_handle_print.cpp
 * Purpose: Implement AstOptimizer::optimizePrintStmt handler.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <memory>
#include <vector>

using namespace gwbasic;

auto AstOptimizer::optimizePrintStmt(std::unique_ptr<Stmt>& stmt,
                                     std::vector<std::unique_ptr<Stmt>>& out) -> bool {
    const auto pr = dyn_cast<PrintStmt>(stmt.get());
    if (!pr) return false;
    if (pr->value) pr->value = optExpr(std::move(pr->value));
    for (auto& v : pr->more) v = optExpr(std::move(v));
    out.emplace_back(std::move(stmt));
    return true;
}
