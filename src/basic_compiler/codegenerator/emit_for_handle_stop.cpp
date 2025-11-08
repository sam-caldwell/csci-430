// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForHandleStop
 * Purpose: Emit STOP handling inside a FOR body (prints break message and exits).
 */
void CodeGenerator::emitForHandleStop(std::ostringstream& out) {
    std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt) << Symbols::LF;
    out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_) << Symbols::LF;
    out << "  br label %exit" << Symbols::LF;
}

} // namespace gwbasic

