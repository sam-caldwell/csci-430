// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/*
 * Function: scanStmtForRnd
 * Summary: Scan a statement to detect RND usage.
 * Parameters:
 *  - s: Statement node to scan.
 * Returns:
 *  - void (sets internal flag when RND is referenced)
 */
void CodeGenerator::scanStmtForRnd(const Stmt* stmt) {
    if (stmt == nullptr) {
        return;
    }
    if (const auto* printStmt = dyn_cast<const PrintStmt>(stmt)) {
        if (printStmt->value) {
            scanExprForRnd(printStmt->value.get());
        }
        for (const auto& valExpr : printStmt->more) {
            scanExprForRnd(valExpr.get());
        }
    } else if (const auto* assign = dyn_cast<const AssignStmt>(stmt)) {
        scanExprForRnd(assign->value.get());
    } else if (const auto* ifStmt = dyn_cast<const IfStmt>(stmt)) {
        scanExprForRnd(ifStmt->cond.get());
    } else if (const auto* forStmt = dyn_cast<const ForStmt>(stmt)) {
        scanExprForRnd(forStmt->start.get());
        scanExprForRnd(forStmt->end.get());
        if (forStmt->step) {
            scanExprForRnd(forStmt->step.get());
        }
        for (const auto& bodyStmt : forStmt->body) {
            scanStmtForRnd(bodyStmt.get());
        }
    } else if (const auto* randStmt = dyn_cast<const RandomizeStmt>(stmt)) {
        scanExprForRnd(randStmt->seed.get());
    }
}

} // namespace gwbasic
