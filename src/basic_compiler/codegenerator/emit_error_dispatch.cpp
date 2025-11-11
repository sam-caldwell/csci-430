// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitErrorDispatch
 * Summary: Set error-related globals and dispatch to handler or exit.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - errCode: BASIC error code.
 *  - lineNo: Source line number at error.
 *  - stmtIndex: Statement index within line.
 * Returns:
 *  - void
 */
void CodeGenerator::emitErrorDispatch(std::ostringstream& out, int errCode, int lineNo, int stmtIndex) {
    out << std::format("  store i32 {}, ptr @gwb_err_code", errCode) << Symbols::LF;
    out << std::format("  store i32 {}, ptr @gwb_err_line", lineNo) << Symbols::LF;
    out << std::format("  store i32 {}, ptr @gwb_resume_line", lineNo) << Symbols::LF;
    out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
    out << std::format("  store i1 true, ptr @gwb_in_handler") << Symbols::LF;
    std::string trap = nextTemp();
    out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
    out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
    for (const auto & [lnum, line_ptr] : lineMap_) {
        (void)line_ptr;
        out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF;
    }
    out << "  ]" << Symbols::LF;
}

} // namespace gwbasic
