// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_optimize_for_body.cpp
 * Purpose: Implement AstOptimizer::optimizeForBody to simplify FOR bodies.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <memory>
#include <utility>
#include <vector>

using namespace gwbasic;

// NOLINTBEGIN(readability-function-size)
void AstOptimizer::optimizeForBody(ForStmt& forStmt) {
    std::vector<std::unique_ptr<Stmt>> body;
    body.reserve(forStmt.body.size());
    for (auto& stmt : forStmt.body) {
        if (auto* const assign = dyn_cast<AssignStmt>(stmt.get())) {
            assign->value = optExpr(std::move(assign->value));
            body.emplace_back(std::move(stmt));
            continue;
        }
        if (auto* const printStmt = dyn_cast<PrintStmt>(stmt.get())) {
            if (printStmt->value) {
                printStmt->value = optExpr(std::move(printStmt->value));
            }
            for (auto& exprItem : printStmt->more) {
                exprItem = optExpr(std::move(exprItem));
            }
            body.emplace_back(std::move(stmt));
            continue;
        }
        body.emplace_back(std::move(stmt));
    }
    forStmt.body = std::move(body);
}
// NOLINTEND(readability-function-size)
