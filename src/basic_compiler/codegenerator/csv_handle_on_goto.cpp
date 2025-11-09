// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OnGotoStmt.h"

namespace gwbasic {

/* Collect variables for ON ... GOTO */
void CodeGenerator::csvHandleOnGoto(const OnGotoStmt* onGotoStmt) {
    collectExprVars(onGotoStmt->index.get());
    logSem() << "OnGoto targets=" << onGotoStmt->targets.size() << Symbols::LF;
}

} // namespace gwbasic
