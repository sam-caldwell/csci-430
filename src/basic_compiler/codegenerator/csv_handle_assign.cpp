// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/AssignStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleAssign
 * Summary: Collect variables referenced by a simple assignment.
 * Parameters:
 *  - assignStmt: Parsed AssignStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleAssign(const AssignStmt* assignStmt) {
    variables_.insert(assignStmt->name);
    collectExprVars(assignStmt->value.get());
    logSem() << "Assign " << assignStmt->name << " @ "
             << assignStmt->pos.line << ':' << assignStmt->pos.col
             << Symbols::LF;
}

} // namespace gwbasic
