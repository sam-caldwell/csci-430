// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_handle_print.cpp
 * Purpose: Implement AstOptimizer::optimizePrintStmt handler.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <memory>
#include <utility>
#include <vector>

using namespace gwbasic;

auto AstOptimizer::optimizePrintStmt(std::unique_ptr<Stmt>& statement,
                                     std::vector<std::unique_ptr<Stmt>>& out) -> bool {
    auto* const printStmt = dyn_cast<PrintStmt>(statement.get());
    if (printStmt == nullptr) {
        return false;
    }
    if (printStmt->value) {
        printStmt->value = optExpr(std::move(printStmt->value));
    }
    for (auto& expr : printStmt->more) {
        expr = optExpr(std::move(expr));
    }
    out.emplace_back(std::move(statement));
    return true;
}
