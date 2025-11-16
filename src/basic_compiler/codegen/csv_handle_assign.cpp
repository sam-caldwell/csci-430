// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/AssignStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleAssign
 * Summary: Collect variables referenced by a simple assignment.
 * Parameters:
 *  - assign_stmt: Parsed AssignStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleAssign(const AssignStmt* assign_stmt) {
    variables_.insert(assign_stmt->name);
    collectExprVars(assign_stmt->value.get());
    
}

} // namespace gwbasic
