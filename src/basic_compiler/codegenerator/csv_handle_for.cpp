// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ForStmt.h"

namespace gwbasic {

/* Collect variables for FOR header and nested body. */
void CodeGenerator::csvHandleFor(const ForStmt* forStmt) {
    variables_.insert(forStmt->var);
    collectExprVars(forStmt->start.get());
    collectExprVars(forStmt->end.get());
    if (forStmt->step) {
        collectExprVars(forStmt->step.get());
    }
    for (const auto& bodyStmt : forStmt->body) {
        collectStmtVars(bodyStmt.get());
    }
    logSem() << "For var=" << forStmt->var << " @ "
             << forStmt->pos.line << ':' << forStmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
