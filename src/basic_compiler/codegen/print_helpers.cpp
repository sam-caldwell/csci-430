// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>

namespace gwbasic {

CodeGenerator::PrintBuffer CodeGenerator::snprintfSbufI64(std::ostringstream &out,
                                                          const std::string &fmtReg,
                                                          const std::string &i64Reg) {
    std::string sbuf = nextTemp();
    out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
    std::string n = nextTemp();
    out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})",
                       n, sbuf, fmtReg, i64Reg)
        << Symbols::LF;
    std::string n64 = nextTemp();
    out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
    return {sbuf, n64};
}

CodeGenerator::PrintBuffer CodeGenerator::snprintfSbufDouble(std::ostringstream &out,
                                                             const std::string &fmtReg,
                                                             const std::string &dblReg) {
    std::string sbuf = nextTemp();
    out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
    std::string n = nextTemp();
    out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, double {})",
                       n, sbuf, fmtReg, dblReg)
        << Symbols::LF;
    std::string n64 = nextTemp();
    out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
    return {sbuf, n64};
}

CodeGenerator::PrintBuffer CodeGenerator::snprintfSbufStr(std::ostringstream &out,
                                                          const std::string &fmtReg,
                                                          const std::string &strReg) {
    std::string sbuf = nextTemp();
    out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
    std::string n = nextTemp();
    out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})",
                       n, sbuf, fmtReg, strReg)
        << Symbols::LF;
    std::string n64 = nextTemp();
    out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
    return {sbuf, n64};
}

void CodeGenerator::emitScreenWrite(std::ostringstream &out,
                                    const std::string &sbufReg,
                                    const std::string &n64Reg) {
    out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbufReg, n64Reg) << Symbols::LF;
}

std::string CodeGenerator::getFmtNumPtr(std::ostringstream &out,
                                        const bool addNewline,
                                        const bool nextStartsWithSpace) {
    const char *sym = addNewline ? "@.fmt_num"
                                 : (nextStartsWithSpace ? "@.fmt_num_ns" : "@.fmt_num_sp");
    std::string fmt = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, sym) << Symbols::LF;
    return fmt;
}

std::string CodeGenerator::getFmtIntPtr(std::ostringstream &out,
                                        const bool addNewline,
                                        const bool nextStartsWithSpace) {
    const char *sym = addNewline ? "@.fmt_int"
                                 : (nextStartsWithSpace ? "@.fmt_int_ns" : "@.fmt_int_sp");
    std::string fmt = nextTemp();
    out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, sym) << Symbols::LF;
    return fmt;
}

} // namespace gwbasic
