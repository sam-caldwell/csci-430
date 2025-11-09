// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/IfStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleIf
 * Summary: Collect variables referenced by an IF condition.
 * Parameters:
 *  - ifStmt: Parsed IfStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleIf(const IfStmt* ifStmt) {
    collectExprVars(ifStmt->cond.get());
    logSem() << "If @ " << ifStmt->pos.line << ':' << ifStmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
