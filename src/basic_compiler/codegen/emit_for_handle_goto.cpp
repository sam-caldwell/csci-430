// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitForHandleGoto
 * Summary: Emit IR for GOTO inside a FOR body and signal termination.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - gt: Parsed GotoStmt node.
 * Returns:
 *  - bool: Always true (handler terminates the loop body).
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool CodeGenerator::emitForHandleGoto(std::ostringstream& out, const GotoStmt* goto_stmt) {
    const std::string ir_text = std::format("  br label %{}", lineLabelName(goto_stmt->targetLine));
    out << ir_text << Symbols::LF;
    return true;
}

} // namespace gwbasic
