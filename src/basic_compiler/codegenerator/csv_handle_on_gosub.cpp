// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for ON ... GOSUB */
void CodeGenerator::csvHandleOnGosub(const OnGosubStmt* ogs) {
    collectExprVars(ogs->index.get());
    logSem() << "OnGosub targets=" << ogs->targets.size() << Symbols::LF;
}

} // namespace gwbasic

