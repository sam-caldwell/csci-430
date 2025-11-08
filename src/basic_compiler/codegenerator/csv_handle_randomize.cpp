// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for RANDOMIZE. */
void CodeGenerator::csvHandleRandomize(const RandomizeStmt* rz) {
    if (rz->seed) collectExprVars(rz->seed.get());
    logSem() << "Randomize @ " << rz->pos.line << ':' << rz->pos.col << Symbols::LF;
}

} // namespace gwbasic

