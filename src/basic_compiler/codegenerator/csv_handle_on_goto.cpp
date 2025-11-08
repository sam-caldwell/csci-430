// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for ON ... GOTO */
void CodeGenerator::csvHandleOnGoto(const OnGotoStmt* og) {
    collectExprVars(og->index.get());
    logSem() << "OnGoto targets=" << og->targets.size() << Symbols::LF;
}

} // namespace gwbasic

