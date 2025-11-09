// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/RandomizeStmt.h"

namespace gwbasic {

/* Collect variables for RANDOMIZE. */
void CodeGenerator::csvHandleRandomize(const RandomizeStmt* randomizeStmt) {
    if (randomizeStmt->seed) {
        collectExprVars(randomizeStmt->seed.get());
    }
    logSem() << "Randomize @ " << randomizeStmt->pos.line << ':'
             << randomizeStmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
