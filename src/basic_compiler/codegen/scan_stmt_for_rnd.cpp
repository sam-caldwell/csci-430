// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
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
void CodeGenerator::scanStmtForRnd(const Stmt* s) {
    if (!s) return;
    if (const auto p = dyn_cast<const PrintStmt>(s)) {
        if (p->value)
            scanExprForRnd(p->value.get());
        for (const auto& v : p->more)
            scanExprForRnd(v.get());
    } else if (const auto a = dyn_cast<const AssignStmt>(s)) {
        scanExprForRnd(a->value.get());
    } else if (const auto i = dyn_cast<const IfStmt>(s)) {
        scanExprForRnd(i->cond.get());
    } else if (const auto f = dyn_cast<const ForStmt>(s)) {
        scanExprForRnd(f->start.get());
        scanExprForRnd(f->end.get());
        if (f->step)
            scanExprForRnd(f->step.get());
        for (const auto& bs : f->body)
            scanStmtForRnd(bs.get());
    } else if (const auto rz = dyn_cast<const RandomizeStmt>(s)) {
        scanExprForRnd(rz->seed.get());
    }
}

} // namespace gwbasic
