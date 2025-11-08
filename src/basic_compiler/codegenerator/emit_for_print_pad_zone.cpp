// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForPrintPadZone
 * Purpose: Emit IR to pad to the next PRINT zone when using comma separators.
 */
void CodeGenerator::emitForPrintPadZone(std::ostringstream& out, const PrintStmt* pr) {
    if (!printZones_) return;
    auto col = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_cur_col", col) << Symbols::LF;
    auto mod = nextTemp(); out << std::format("  {} = srem i32 {}, 14", mod, col) << Symbols::LF;
    auto isZero = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", isZero, mod) << Symbols::LF;
    auto sub = nextTemp(); out << std::format("  {} = sub i32 14, {}", sub, mod) << Symbols::LF;
    auto pad = nextTemp(); out << std::format("  {} = select i1 {}, i32 14, i32 {}", pad, isZero, sub) << Symbols::LF;
    auto fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_pad, i64 0", fmt) << Symbols::LF;
    auto spaces = nextTemp(); out << std::format("  {} = getelementptr inbounds [15 x i8], ptr @.spaces_14, i64 0, i64 0", spaces) << Symbols::LF;
    if (pr->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF;
        auto fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i32 {}, ptr {})", fh, fmt, pad, spaces) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {}, ptr {})", fmt, pad, spaces) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i32 {}, ptr {})", n, sbuf, fmt, pad, spaces) << Symbols::LF;
        auto n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
    }
}

} // namespace gwbasic

