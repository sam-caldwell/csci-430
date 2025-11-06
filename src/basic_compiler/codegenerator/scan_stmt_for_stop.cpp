// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/StopStmt.h"

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
void CodeGenerator::scanStmtForStop(const Stmt* s) {
    if (!s || needsBreakMsg_) return;
    if (isa<const StopStmt>(s)) { needsBreakMsg_ = true; return; }
    if (const auto ib = dyn_cast<const IfBlockStmt>(s)) {
        for (const auto& st : ib->thenBody) scanStmtForStop(st.get());
        for (const auto& st : ib->elseBody) scanStmtForStop(st.get());
        return;
    }
    if (const auto f = dyn_cast<const ForStmt>(s)) {
        for (const auto& st : f->body) scanStmtForStop(st.get());
        return;
    }
    if (const auto w = dyn_cast<const WhileStmt>(s)) {
        for (const auto& st : w->body) scanStmtForStop(st.get());
        return;
    }
}

} // namespace gwbasic

