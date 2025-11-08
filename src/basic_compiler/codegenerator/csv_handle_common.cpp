// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for COMMON. */
void CodeGenerator::csvHandleCommon(const CommonStmt* cs) {
    for (const auto& n : cs->names) {
        variables_.insert(n);
        commonVariables_.insert(n);
        logSem() << "Common " << n << " @ " << cs->pos.line << ':' << cs->pos.col << Symbols::LF;
    }
}

} // namespace gwbasic

