// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/MidAssignStmt.h"

namespace gwbasic {

/*
 * Function: emitSubHandleMidAssign
 * Summary: Emit MID$ slice assignment in a subroutine (delegates to FOR helper).
 * Parameters:
 *  - out: IR output stream.
 *  - mid: MidAssignStmt node.
 *  - entryLabel: Current subroutine entry label (for label naming and expressions).
 *  - localCounter: Per-line counter for unique labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitSubHandleMidAssign(std::ostringstream& out, const MidAssignStmt* mid, std::string_view entryLabel, int& localCounter) {
    // Reuse the FOR-body helper which handles indices, bounds, and stores.
    emitForHandleMidAssign(out, mid, std::string(entryLabel), localCounter);
}

} // namespace gwbasic

