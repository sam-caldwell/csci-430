// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/MidAssignStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleMidAssign
 * Summary: Collect variables referenced by MID$ assignment (scalar/array).
 * Parameters:
 *  - mid_stmt: Parsed MidAssignStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleMidAssign(const MidAssignStmt* mid_stmt) {
    variables_.insert(mid_stmt->name);
    for (const auto& indexExpr : mid_stmt->indices) {
        collectExprVars(indexExpr.get());
    }
    collectExprVars(mid_stmt->start.get());
    if (mid_stmt->len) {
        collectExprVars(mid_stmt->len.get());
    }
    collectExprVars(mid_stmt->value.get());
    logSem() << "MidAssign " << mid_stmt->name << " @ "
             << mid_stmt->pos.line << ':' << mid_stmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
