// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ForStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleFor
 * Summary: Collect variables referenced by a FOR header and body.
 * Parameters:
 *  - for_stmt: Parsed ForStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleFor(const ForStmt* for_stmt) {
    variables_.insert(for_stmt->var);
    collectExprVars(for_stmt->start.get());
    collectExprVars(for_stmt->end.get());
    if (for_stmt->step) {
        collectExprVars(for_stmt->step.get());
    }
    for (const auto& bodyStmt : for_stmt->body) {
        collectStmtVars(bodyStmt.get());
    }
    logSem() << "For var=" << for_stmt->var << " @ "
             << for_stmt->pos.line << ':' << for_stmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
