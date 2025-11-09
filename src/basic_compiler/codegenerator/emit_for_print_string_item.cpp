// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

/*
 * Function: emitForPrintStringItem
 * Summary: Emit IR for printing a string item within PRINT.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - pr: Parsed PrintStmt node for context (channel, separators, trail).
 *  - se: String expression to print.
 *  - addNL: Whether to print a trailing newline for this item.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForPrintStringItem(std::ostringstream& out, const PrintStmt* pr, const StringExpr* se, bool addNL) {
    std::string sptr = nextTemp();
    if (se->value.empty()) {
        out << std::format("  {} = getelementptr inbounds i8, ptr @.empty, i64 0", sptr) << Symbols::LF;
    } else {
        int id = strLiteralId_[se->value];
        out << std::format("  {} = getelementptr inbounds [{} x i8], ptr {}, i64 0, i64 0", sptr, (se->value.size() + 1), globalStringName(id)) << Symbols::LF;
    }
    auto fmt = nextTemp();
    if (pr->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF;
        auto fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
        out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (addNL ? "@.fmt_str" : "@.fmt_str_sp")) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, fmt, sptr) << Symbols::LF;
    } else {
        out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (addNL ? "@.fmt_str" : "@.fmt_str_sp")) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, fmt, sptr) << Symbols::LF;
        auto n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
    }
}

} // namespace gwbasic
