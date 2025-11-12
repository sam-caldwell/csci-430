// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
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
    if (const auto* const printStmt = dyn_cast<const PrintStmt>(stmt)) {
        csvHandlePrint(printStmt);
        return;
    }
    if (const auto* const assignStmt = dyn_cast<const AssignStmt>(stmt)) {
        csvHandleAssign(assignStmt);
        return;
    }
    if (const auto* const midAssign = dyn_cast<const MidAssignStmt>(stmt)) {
        csvHandleMidAssign(midAssign);
        return;
    }
    if (const auto* const ifStmt = dyn_cast<const IfStmt>(stmt)) {
        csvHandleIf(ifStmt);
        return;
    }
    if (const auto* const forStmt = dyn_cast<const ForStmt>(stmt)) {
        csvHandleFor(forStmt);
        return;
    }
    if (const auto* const inputStmt = dyn_cast<const InputStmt>(stmt)) {
        csvHandleInput(inputStmt);
        return;
    }
    if (const auto* const randStmt = dyn_cast<const RandomizeStmt>(stmt)) {
        csvHandleRandomize(randStmt);
        return;
    }
    if (const auto* const commonStmt = dyn_cast<const CommonStmt>(stmt)) {
        csvHandleCommon(commonStmt);
        return;
    }
    if (dyn_cast<const MergeStmt>(stmt) != nullptr) {
        return; // no-op
    }
    if (const auto* const dataStmt = dyn_cast<const DataStmt>(stmt)) {
        csvHandleData(dataStmt);
        return;
    }
    if (const auto* const readStmt = dyn_cast<const ReadStmt>(stmt)) {
        csvHandleRead(readStmt);
        return;
    }
    if (const auto* const onGoto = dyn_cast<const OnGotoStmt>(stmt)) {
        csvHandleOnGoto(onGoto);
        return;
    }
    if (const auto* const onGosub = dyn_cast<const OnGosubStmt>(stmt)) {
        csvHandleOnGosub(onGosub);
        return;
    }
}

} // namespace gwbasic
