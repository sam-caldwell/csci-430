// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForPrintDynamicAuto
 * Purpose: PRINT numeric with automatic int/float formatting.
 */
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitForPrintDynamicAuto(std::ostringstream& out, const PrintStmt* pr, const std::string& val,
                                            bool addNL, bool nextStartsWithSpace,
                                            const std::string& currLineLabel, int& localCounter) {
    auto fmtF = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, (addNL ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp"))) << Symbols::LF;
    auto fmtI = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, (addNL ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp"))) << Symbols::LF;
    auto iv = nextTemp(); out << std::format("  {} = fptosi double {} to i64", iv, val) << Symbols::LF;
    auto dv = nextTemp(); out << std::format("  {} = sitofp i64 {} to double", dv, iv) << Symbols::LF;
    auto isInt = nextTemp(); out << std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val) << Symbols::LF;
    auto intLbl = currLineLabel + std::string("_print_int_") + std::to_string(++localCounter);
    auto fltLbl = currLineLabel + std::string("_print_flt_") + std::to_string(localCounter);
    auto contLbl = currLineLabel + std::string("_print_cont_") + std::to_string(localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl) << Symbols::LF;
    out << intLbl << ":" << Symbols::LF;
    if (pr->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF;
        auto fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, fmtI, iv) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", n, sbuf, fmtI, iv) << Symbols::LF;
        auto n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << fltLbl << ":" << Symbols::LF;
    if (pr->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF;
        auto fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, fmtF, val) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})", n, sbuf, fmtF, val) << Symbols::LF;
        auto n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
