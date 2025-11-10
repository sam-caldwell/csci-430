// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/PrintStmt.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitForPrintDynamicAuto
 * Summary: PRINT numeric item with automatic int/float formatting.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - pr: Parsed PrintStmt node.
 *  - val: SSA name of numeric value to print (double).
 *  - addNL: Whether to print a trailing newline for this item.
 *  - nextStartsWithSpace: Whether the next item begins with a space.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitForPrintDynamicAuto(std::ostringstream& out, const PrintStmt* printStmt, const std::string& val,
                                            bool addNL, bool nextStartsWithSpace,
                                            std::string_view currLineLabel, int& localCounter) {
    auto fmtF = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, (addNL ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp"))) << Symbols::LF;
    auto fmtI = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, (addNL ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp"))) << Symbols::LF;
    auto intValReg = nextTemp(); out << std::format("  {} = fptosi double {} to i64", intValReg, val) << Symbols::LF;
    auto doubleFromInt = nextTemp(); out << std::format("  {} = sitofp i64 {} to double", doubleFromInt, intValReg) << Symbols::LF;
    auto isInt = nextTemp(); out << std::format("  {} = fcmp oeq double {}, {}", isInt, doubleFromInt, val) << Symbols::LF;
    auto intLbl = std::format("{}_print_int_{}", currLineLabel, ++localCounter);
    auto fltLbl = std::format("{}_print_flt_{}", currLineLabel, localCounter);
    auto contLbl = std::format("{}_print_cont_{}", currLineLabel, localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl) << Symbols::LF;
    out << intLbl << ":" << Symbols::LF;
    if (printStmt->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, printStmt->channel - 1) << Symbols::LF;
        auto fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fileHandle, fmtI, intValReg) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, intValReg) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto count = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", count, sbuf, fmtI, intValReg) << Symbols::LF;
        auto count64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", count64, count) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, count64) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << fltLbl << ":" << Symbols::LF;
    if (printStmt->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, printStmt->channel - 1) << Symbols::LF;
        auto fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fileHandle, fmtF, val) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto count = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", count, sbuf, fmtF, val) << Symbols::LF;
        auto count64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", count64, count) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, count64) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
