// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/CommonStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleCommon
 * Summary: Collect variables declared by COMMON.
 * Parameters:
 *  - commonStmt: Parsed CommonStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleCommon(const CommonStmt* commonStmt) {
    for (const auto& name : commonStmt->names) {
        variables_.insert(name);
        commonVariables_.insert(name);
        logSem() << "Common " << name << " @ " << commonStmt->pos.line << ':' << commonStmt->pos.col << Symbols::LF;
    }
}

} // namespace gwbasic
