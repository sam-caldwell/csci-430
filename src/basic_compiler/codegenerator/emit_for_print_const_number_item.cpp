// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"
#include <cmath>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForPrintConstNumberItem
 * Purpose: Emit IR for printing a constant numeric item.
 */
void CodeGenerator::emitForPrintConstNumberItem(std::ostringstream& out, const PrintStmt* pr, double cv, bool addNL, bool nextStartsWithSpace) {
    if (std::floor(cv) == cv) {
        auto fmtI = nextTemp();
        const char* symI = addNL ? "@.fmt_int" : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp");
        out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, symI) << Symbols::LF;
        auto iv = static_cast<long long>(cv);
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
        return;
    }
    auto fmtF = nextTemp();
    const char* symF = addNL ? "@.fmt_num" : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp");
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, symF) << Symbols::LF;
    if (pr->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF;
        auto fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {:.6f})", fh, fmtF, cv) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {:.6f})", fmtF, cv) << Symbols::LF;
        auto sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
        auto n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {:.6f})", n, sbuf, fmtF, cv) << Symbols::LF;
        auto n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
        out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
    }
}

} // namespace gwbasic

