// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"

namespace gwbasic {

/*
 * Function: emitSubHandlePrint
 * Summary: Emit PRINT inside a subroutine (reusing FOR print helpers).
 * Parameters:
 *  - out: IR output stream.
 *  - pr: PrintStmt node.
 *  - entryLabel: Current subroutine entry label.
 *  - localCounter: Per-line counter for unique labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitSubHandlePrint(std::ostringstream& out, const PrintStmt* pr, std::string_view entryLabel, int& localCounter) {
    emitForHandlePrint(out, pr, std::string(entryLabel), localCounter);
}

} // namespace gwbasic

