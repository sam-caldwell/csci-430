// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/Symbols.h"
#include <sstream>
#include <format>

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
void CodeGenerator::emitSubHandleInput(std::ostringstream& out, const InputStmt* ins, std::string_view entryLabel) {
    (void)entryLabel;
    if (ins->promptLiteral || ins->promptVar) {
        std::string pstr;
        if (ins->promptLiteral) {
            if (strLiteralId_.contains(*ins->promptLiteral)) {
                int id = strLiteralId_[*ins->promptLiteral];
                pstr = nextTemp();
                { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", pstr, globalStringName(id)); out << ir << Symbols::LF; }
            }
        } else {
            ensureVarAllocated(out, *ins->promptVar);
            pstr = nextTemp(); { std::string ir = std::format("  {} = load ptr, ptr {}", pstr, varAllocaName_[*ins->promptVar]); out << ir << Symbols::LF; }
            std::string safe = nextTemp(); { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, pstr); out << ir << Symbols::LF; } pstr = safe;
        }
        if (!pstr.empty()) {
            std::string fmtS = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str_sp, i64 0", fmtS); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmtS, pstr); out << ir << Symbols::LF; }
        }
    }
    for (const auto& vname : ins->variables) {
        std::string fmt = nextTemp(); { std::string ir1 = std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt); out << ir1 << Symbols::LF; }
        std::string tmp = nextTemp(); { std::string ir = std::format("  {} = alloca double", tmp); out << ir << Symbols::LF; }
        std::string ir2 = std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, tmp);
        out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InputStmt -> " << ir2 << Symbols::LF;
        std::string dv = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", dv, tmp); out << ir << Symbols::LF; }
        storeNumberToVar(out, vname, dv);
    }
}

} // namespace gwbasic
