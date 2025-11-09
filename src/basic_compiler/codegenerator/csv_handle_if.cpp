// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/IfStmt.h"

namespace gwbasic {

/* Collect variables for IF condition. */
void CodeGenerator::csvHandleIf(const IfStmt* ifStmt) {
    collectExprVars(ifStmt->cond.get());
    logSem() << "If @ " << ifStmt->pos.line << ':' << ifStmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
