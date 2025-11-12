// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/IfStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleIf
 * Summary: Collect variables referenced by an IF condition.
 * Parameters:
 *  - if_stmt: Parsed IfStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleIf(const IfStmt* if_stmt) {
    collectExprVars(if_stmt->cond.get());
    logSem() << "If @ " << if_stmt->pos.line << ':' << if_stmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
