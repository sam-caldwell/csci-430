// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for MID$ assignment (scalar or array element). */
void CodeGenerator::csvHandleMidAssign(const MidAssignStmt* m) {
    variables_.insert(m->name);
    for (const auto& ix : m->indices) collectExprVars(ix.get());
    collectExprVars(m->start.get());
    if (m->len) collectExprVars(m->len.get());
    collectExprVars(m->value.get());
    logSem() << "MidAssign " << m->name << " @ " << m->pos.line << ':' << m->pos.col << Symbols::LF;
}

} // namespace gwbasic

