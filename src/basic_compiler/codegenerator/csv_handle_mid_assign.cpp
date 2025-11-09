// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/MidAssignStmt.h"

namespace gwbasic {

/* Collect variables for MID$ assignment (scalar or array element). */
void CodeGenerator::csvHandleMidAssign(const MidAssignStmt* midAssign) {
    variables_.insert(midAssign->name);
    for (const auto& indexExpr : midAssign->indices) {
        collectExprVars(indexExpr.get());
    }
    collectExprVars(midAssign->start.get());
    if (midAssign->len) {
        collectExprVars(midAssign->len.get());
    }
    collectExprVars(midAssign->value.get());
    logSem() << "MidAssign " << midAssign->name << " @ "
             << midAssign->pos.line << ':' << midAssign->pos.col << Symbols::LF;
}

} // namespace gwbasic
