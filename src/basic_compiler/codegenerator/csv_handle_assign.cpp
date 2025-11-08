// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for simple assignment. */
void CodeGenerator::csvHandleAssign(const AssignStmt* a) {
    variables_.insert(a->name);
    collectExprVars(a->value.get());
    logSem() << "Assign " << a->name << " @ " << a->pos.line << ':' << a->pos.col << Symbols::LF;
}

} // namespace gwbasic

