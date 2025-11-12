// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitForHandleStop
 * Summary: Emit STOP handling inside a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleStop(std::ostringstream& out) {
    const std::string fmt = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt) << Symbols::LF
        << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_) << Symbols::LF
        << "  br label %exit" << Symbols::LF;
}

} // namespace gwbasic
