// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/GosubStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: emitForHandleGosub
 * Summary: Emit inline GOSUB handling within a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - gs: Parsed GosubStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleGosub(std::ostringstream& out, const GosubStmt* gs, const std::string& currLineLabel, int& localCounter) {
    std::string contLbl = currLineLabel + std::string("_gosub_cont") + std::to_string(++localCounter);
    std::string entryLbl = currLineLabel + std::string("_gosub_entry") + std::to_string(localCounter);
    out << "  br label %" << entryLbl << Symbols::LF;
    emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
