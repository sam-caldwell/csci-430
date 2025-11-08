// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_handle_assign.cpp
 * Purpose: Implement AstOptimizer::optimizeAssignStmt handler.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/AssignStmt.h"

using namespace gwbasic;

auto AstOptimizer::optimizeAssignStmt(std::unique_ptr<Stmt>& st,
                                      std::vector<std::unique_ptr<Stmt>>& out) -> bool {
    const auto asg = dyn_cast<AssignStmt>(st.get());
    if (!asg) return false;
    asg->value = optExpr(std::move(asg->value));
    out.emplace_back(std::move(st));
    return true;
}

