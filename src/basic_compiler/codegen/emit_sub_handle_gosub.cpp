// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/GosubStmt.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitSubHandleGosub
 * Summary: Emit nested GOSUB by inlining the target and creating a continuation.
 * Parameters:
 *  - out: IR output stream.
 *  - gosubStmt: GosubStmt node.
 *  - entryLabel: Current subroutine entry label.
 *  - localCounter: Per-line counter for unique labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitSubHandleGosub(std::ostringstream& out, const GosubStmt* gosubStmt, std::string_view entryLabel, int& localCounter) {
    const std::string cont = std::format("{}_gosub_cont{}", entryLabel, ++localCounter);
    const std::string ent = std::format("{}_gosub_entry{}", entryLabel, localCounter);
    {
        const std::string irLine = std::format("  br label %{}", ent);
        out << irLine << Symbols::LF;
    
    }
    emitSubroutineInline(out, gosubStmt->targetLine, ent, cont);
    out << cont << ":" << Symbols::LF;
}

} // namespace gwbasic
