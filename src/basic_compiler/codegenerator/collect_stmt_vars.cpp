// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::collectStmtVars
 * Inputs:
 *  - s: statement node to analyze
 * Outputs:
 *  - void (updates internal sets/maps of variables and string literals)
 * Theory of operation:
 *  - Inspects the statement kind to discover referenced variables and
 *    string constants, recursing into contained expressions/blocks.
 */
void CodeGenerator::collectStmtVars(const Stmt* s) {
    if (const auto p = dyn_cast<const PrintStmt>(s))
        return csvHandlePrint(p);
    if (const auto a = dyn_cast<const AssignStmt>(s))
        return csvHandleAssign(a);
    if (const auto m = dyn_cast<const MidAssignStmt>(s))
        return csvHandleMidAssign(m);
    if (const auto i = dyn_cast<const IfStmt>(s))
        return csvHandleIf(i);
    if (const auto f = dyn_cast<const ForStmt>(s))
        return csvHandleFor(f);
    if (const auto in = dyn_cast<const InputStmt>(s))
        return csvHandleInput(in);
    if (const auto rz = dyn_cast<const RandomizeStmt>(s))
        return csvHandleRandomize(rz);
    if (const auto cs = dyn_cast<const CommonStmt>(s))
        return csvHandleCommon(cs);
    if (dyn_cast<const MergeStmt>(s))
        return; // no-op
    if (const auto ds = dyn_cast<const DataStmt>(s))
        return csvHandleData(ds);
    if (const auto rd = dyn_cast<const ReadStmt>(s))
        return csvHandleRead(rd);
    if (const auto og = dyn_cast<const OnGotoStmt>(s))
        return csvHandleOnGoto(og);
    if (const auto ogs = dyn_cast<const OnGosubStmt>(s))
        return csvHandleOnGosub(ogs);
}

} // namespace gwbasic
