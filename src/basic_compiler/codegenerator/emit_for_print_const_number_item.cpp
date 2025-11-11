// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <cmath>
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitForPrintConstNumberItem
 * Summary: Emit IR for printing a constant numeric item.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - pr: Parsed PrintStmt node.
 *  - cv: Constant numeric value.
 *  - addNL: Whether to print a trailing newline.
 *  - nextStartsWithSpace: Whether next item begins with a space.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void CodeGenerator::emitForPrintConstNumberItem(std::ostringstream& out, const PrintStmt* print_stmt, double const_value, bool addNL, bool nextStartsWithSpace) {
    if (std::floor(const_value) == const_value) {
        const std::string fmtI = nextTemp();
        const char* symI = nullptr;
        if (addNL) {
            symI = "@.fmt_int";
        } else {
            symI = nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp";
        }
        out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI) << Symbols::LF;
        const auto int_val = static_cast<long long>(const_value);
        if (print_stmt->channel >= 1) {
            const std::string fptr = nextTemp();
            const std::string file_handle = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, print_stmt->channel - 1) << Symbols::LF
                << std::format("  {} = load ptr, ptr {}", file_handle, fptr) << Symbols::LF
                << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", file_handle, fmtI, int_val) << Symbols::LF;
        } else {
            const std::string sbuf = nextTemp();
            const std::string n_chars = nextTemp();
            const std::string n64 = nextTemp();
            out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, int_val) << Symbols::LF
                << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF
                << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", n_chars, sbuf, fmtI, int_val) << Symbols::LF
                << std::format("  {} = sext i32 {} to i64", n64, n_chars) << Symbols::LF
                << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
        }
        return;
    }
    const std::string fmtF = nextTemp();
    const char* symF = nullptr;
    if (nextStartsWithSpace) {
        symF = addNL ? "@.fmt_num" : "@.fmt_num_ns";
    } else {
        symF = addNL ? "@.fmt_num" : "@.fmt_num_sp";
    }
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, symF) << Symbols::LF;
    if (print_stmt->channel >= 1) {
        const std::string fptr = nextTemp();
        const std::string file_handle = nextTemp();
        out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, print_stmt->channel - 1) << Symbols::LF
            << std::format("  {} = load ptr, ptr {}", file_handle, fptr) << Symbols::LF
            << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {:.6f})", file_handle, fmtF, const_value) << Symbols::LF;
    } else {
        const std::string sbuf = nextTemp();
        const std::string n_chars = nextTemp();
        const std::string n64 = nextTemp();
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {:.6f})", fmtF, const_value) << Symbols::LF
            << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF
            << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {:.6f})", n_chars, sbuf, fmtF, const_value) << Symbols::LF
            << std::format("  {} = sext i32 {} to i64", n64, n_chars) << Symbols::LF
            << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
    }
}

} // namespace gwbasic
