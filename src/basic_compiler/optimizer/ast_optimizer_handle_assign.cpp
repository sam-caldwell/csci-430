// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_handle_assign.cpp
 * Purpose: Implement AstOptimizer::optimizeAssignStmt handler.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <memory>
#include <utility>
#include <vector>

using namespace gwbasic;

auto AstOptimizer::optimizeAssignStmt(std::unique_ptr<Stmt>& statement,
                                      std::vector<std::unique_ptr<Stmt>>& out) -> bool {
    AssignStmt* asg = dyn_cast<AssignStmt>(statement.get());
    if (asg == nullptr) {
        return false;
    }
    asg->value = optExpr(std::move(asg->value));
    out.emplace_back(std::move(statement));
    return true;
}
