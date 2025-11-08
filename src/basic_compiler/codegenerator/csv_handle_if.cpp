// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for IF condition. */
void CodeGenerator::csvHandleIf(const IfStmt* i) {
    collectExprVars(i->cond.get());
    logSem() << "If @ " << i->pos.line << ':' << i->pos.col << Symbols::LF;
}

} // namespace gwbasic

