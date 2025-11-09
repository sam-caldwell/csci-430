// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/WhileStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::scanStmtForStop
 * Inputs:
 *  - s: Statement node to scan
 * Outputs:
 *  - void (sets needsBreakMsg_ when STOP encountered)
 * Theory of operation:
 *  - Recursively walks nested blocks to detect STOP, enabling conditional
 *    emission of the break message global.
 */
void CodeGenerator::scanStmtForStop(const Stmt* stmt) {
    if (stmt == nullptr || needsBreakMsg_) {
        return;
    }
    if (isa<const StopStmt>(stmt)) {
        needsBreakMsg_ = true;
        return;
    }
    if (const auto* const ifBlock = dyn_cast<const IfBlockStmt>(stmt)) {
        for (const auto& stmtElement : ifBlock->thenBody) {
            scanStmtForStop(stmtElement.get());
        }
        for (const auto& stmtElement : ifBlock->elseBody) {
            scanStmtForStop(stmtElement.get());
        }
        return;
    }
    if (const auto* const forStmt = dyn_cast<const ForStmt>(stmt)) {
        for (const auto& stmtElement : forStmt->body) {
            scanStmtForStop(stmtElement.get());
        }
        return;
    }
    if (const auto* const whileStmt = dyn_cast<const WhileStmt>(stmt)) {
        for (const auto& stmtElement : whileStmt->body) {
            scanStmtForStop(stmtElement.get());
        }
        return;
    }
}

} // namespace gwbasic
