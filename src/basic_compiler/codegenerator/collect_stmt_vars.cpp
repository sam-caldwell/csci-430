// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/*
 * Function: collectStmtVars
 * Summary: Collect variables and string literals used by a statement.
 * Parameters:
 *  - s: Statement node to analyze.
 * Returns:
 *  - void
 */
void CodeGenerator::collectStmtVars(const Stmt* stmt) {
    if (const auto p = dyn_cast<const PrintStmt>(stmt))
        return csvHandlePrint(p);
    if (const auto a = dyn_cast<const AssignStmt>(stmt))
        return csvHandleAssign(a);
    if (const auto m = dyn_cast<const MidAssignStmt>(stmt))
        return csvHandleMidAssign(m);
    if (const auto i = dyn_cast<const IfStmt>(stmt))
        return csvHandleIf(i);
    if (const auto f = dyn_cast<const ForStmt>(stmt))
        return csvHandleFor(f);
    if (const auto in = dyn_cast<const InputStmt>(stmt))
        return csvHandleInput(in);
    if (const auto rz = dyn_cast<const RandomizeStmt>(stmt))
        return csvHandleRandomize(rz);
    if (const auto cs = dyn_cast<const CommonStmt>(stmt))
        return csvHandleCommon(cs);
    if (dyn_cast<const MergeStmt>(stmt))
        return; // no-op
    if (const auto ds = dyn_cast<const DataStmt>(stmt))
        return csvHandleData(ds);
    if (const auto rd = dyn_cast<const ReadStmt>(stmt))
        return csvHandleRead(rd);
    if (const auto og = dyn_cast<const OnGotoStmt>(stmt))
        return csvHandleOnGoto(og);
    if (const auto ogs = dyn_cast<const OnGosubStmt>(stmt))
        return csvHandleOnGosub(ogs);
}

} // namespace gwbasic
