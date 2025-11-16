// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OnGosubStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleOnGosub
 * Summary: Collect variables referenced by an ON ... GOSUB statement.
 * Parameters:
 *  - onGosubStmt: Parsed OnGosubStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleOnGosub(const OnGosubStmt* onGosubStmt) {
    collectExprVars(onGosubStmt->index.get());
    
}

} // namespace gwbasic
