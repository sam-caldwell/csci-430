// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForPrintDynamicOverride
 * Purpose: PRINT numeric with user-provided format string.
 */
void CodeGenerator::emitForPrintDynamicOverride(std::ostringstream& out, const PrintStmt* pr, const std::string& val,
                                                const std::string& currLineLabel, int& localCounter) { // NOLINT(bugprone-easily-swappable-parameters)
    auto useFmt = emitExpr(out, pr->format.get(), currLineLabel);
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
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, useFmt, iv) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", useFmt, iv) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << fltLbl << ":" << Symbols::LF;
    if (pr->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF;
        auto fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh, useFmt, val) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
