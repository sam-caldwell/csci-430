// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/InputStmt.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitSubHandleInput
 * Summary: Emit INPUT (optional prompt + scanf) in a subroutine.
 * Parameters:
 *  - out: IR output stream.
 *  - ins: InputStmt node.
 *  - entryLabel: Current subroutine entry label.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitSubHandleInput(std::ostringstream& out, const InputStmt* ins, std::string_view entryLabel) {
    (void)entryLabel;
    if (ins->promptLiteral || ins->promptVar) {
        std::string pstr;
        if (ins->promptLiteral) {
            if (strLiteralId_.contains(*ins->promptLiteral)) {
                const int strId = strLiteralId_[*ins->promptLiteral];
                pstr = nextTemp();
                { const std::string irLine = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", pstr, globalStringName(strId)); out << irLine << Symbols::LF; }
            }
        } else {
            ensureVarAllocated(out, *ins->promptVar);
            pstr = nextTemp();
            {
                const std::string irLine = std::format("  {} = load ptr, ptr {}", pstr, varAllocaName_[*ins->promptVar]);
                out << irLine << Symbols::LF;
            }
            std::string safe = nextTemp();
            {
                const std::string irLine = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, pstr);
                out << irLine << Symbols::LF;
            } pstr = safe;
        }
        if (!pstr.empty()) {
            std::string fmtS = nextTemp();
            {
                const std::string irLine = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str_sp, i64 0", fmtS);
                out << irLine << Symbols::LF;
            }
            {
                const std::string irLine = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmtS, pstr);
                out << irLine << Symbols::LF;
            }
        }
    }
    for (const auto& vname : ins->variables) {
        std::string fmt = nextTemp();
        {
            const std::string irFmt = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt);
            out << irFmt << Symbols::LF;
        }
        std::string tmp = nextTemp();
        {
            const std::string irAlloca = std::format("  {} = alloca double", tmp);
            out << irAlloca << Symbols::LF;
        }
        const std::string irScanf = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp);
        out << irScanf << Symbols::LF;
        
        std::string doubleVal = nextTemp();
        {
            const std::string irLoad = std::format("  {} = load double, ptr {}", doubleVal, tmp);
            out << irLoad << Symbols::LF;
        }
        storeNumberToVar(out, vname, doubleVal);
    }
}

} // namespace gwbasic
