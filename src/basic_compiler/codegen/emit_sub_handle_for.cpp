// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ForStmt.h"
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitSubHandleFor
 * Summary: Emit FOR loop inside a subroutine (delegates to emitFor).
 * Parameters:
 *  - out: IR output stream.
 *  - fs: ForStmt node.
 *  - entryLabel: Current subroutine entry label.
 *  - localCounter: Per-line counter for unique labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitSubHandleFor(std::ostringstream& out, const ForStmt* for_stmt, std::string_view entryLabel, int& localCounter) {
    // Mirror integration marker used in line/block emission
    out << "  ;; For var=" << for_stmt->var << Symbols::LF;
    emitFor(out, for_stmt, std::string(entryLabel), localCounter);
}

} // namespace gwbasic
