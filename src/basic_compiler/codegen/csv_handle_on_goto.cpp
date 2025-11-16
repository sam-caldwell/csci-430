// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OnGotoStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleOnGoto
 * Summary: Collect variables referenced by an ON ... GOTO statement.
 * Parameters:
 *  - onGotoStmt: Parsed OnGotoStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleOnGoto(const OnGotoStmt* onGotoStmt) {
    collectExprVars(onGotoStmt->index.get());
    
}

} // namespace gwbasic
