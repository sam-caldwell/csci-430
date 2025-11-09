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
 * Function: emitForPrintDynamicOverride
 * Summary: PRINT numeric item with user-provided format string.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - pr: Parsed PrintStmt node.
 *  - val: SSA name of numeric value to print (double).
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForPrintDynamicOverride(std::ostringstream& out, const PrintStmt* printStmt, const std::string& val,
                                                std::string_view currLineLabel, int& localCounter) {
    auto useFmt = emitExpr(out, printStmt->format.get(), std::string(currLineLabel));
    auto intVal = nextTemp(); out << std::format("  {} = fptosi double {} to i64", intVal, val) << Symbols::LF;
    auto dblVal = nextTemp(); out << std::format("  {} = sitofp i64 {} to double", dblVal, intVal) << Symbols::LF;
    auto isInt = nextTemp(); out << std::format("  {} = fcmp oeq double {}, {}", isInt, dblVal, val) << Symbols::LF;
    auto intLbl = std::string(currLineLabel) + std::string("_print_int_") + std::to_string(++localCounter);
    auto fltLbl = std::string(currLineLabel) + std::string("_print_flt_") + std::to_string(localCounter);
    auto contLbl = std::string(currLineLabel) + std::string("_print_cont_") + std::to_string(localCounter);
    out << std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, fltLbl) << Symbols::LF;
    out << intLbl << ":" << Symbols::LF;
    if (printStmt->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, printStmt->channel - 1) << Symbols::LF;
        auto fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fileHandle, useFmt, intVal) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", useFmt, intVal) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << fltLbl << ":" << Symbols::LF;
    if (printStmt->channel >= 1) {
        auto fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, printStmt->channel - 1) << Symbols::LF;
        auto fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, fptr) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fileHandle, useFmt, val) << Symbols::LF;
    } else {
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", useFmt, val) << Symbols::LF;
    }
    out << std::format("  br label %{}", contLbl) << Symbols::LF;
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
