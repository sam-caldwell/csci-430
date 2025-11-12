// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/GosubStmt.h"
#include <format>
#include <sstream>
#include <string>

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
void CodeGenerator::emitForHandleGosub(std::ostringstream& out, const GosubStmt* gosubStmt, const std::string& currLineLabel, int& localCounter) {
    const std::string contLbl = std::format("{}_gosub_cont{}", currLineLabel, ++localCounter);
    const std::string entryLbl = std::format("{}_gosub_entry{}", currLineLabel, localCounter);
    out << std::format("  br label %{}", entryLbl) << Symbols::LF;
    emitSubroutineInline(out, gosubStmt->targetLine, entryLbl, contLbl);
    out << std::format("{}:", contLbl) << Symbols::LF;
}

} // namespace gwbasic
