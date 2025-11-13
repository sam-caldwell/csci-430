// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/InputStmt.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length)
void CodeGenerator::emitLineHandleInput(std::ostringstream &out,
                                        const InputStmt *ins,
                                        const std::string &currLineLabel,
                                        int &localCounter) {
    if (ins->promptLiteral || ins->promptVar) {
        std::string pstr;
        if (ins->promptLiteral) {
            ensureVarAllocated(out, *ins->promptLiteral);
            pstr = nextTemp();
            const std::string gid = std::format("@.str.{}", strLiteralId_[*ins->promptLiteral]);
            out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", pstr, gid) << Symbols::LF;
        } else {
            ensureVarAllocated(out, *ins->promptVar);
            pstr = nextTemp();
            out << std::format("  {} = load ptr, ptr {}", pstr, varAllocaName_[*ins->promptVar]) << Symbols::LF;
            const std::string len = nextTemp();
            out << std::format("  {} = call i64 @strlen(ptr {})", len, pstr) << Symbols::LF;
            const std::string gt0 = nextTemp();
            out << std::format("  {} = icmp sgt i64 {}, 0", gt0, len) << Symbols::LF;
            const std::string doLbl = std::format("{}_in_ok_{}", currLineLabel, ++localCounter);
            const std::string contLbl = std::format("{}_in_cont_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", gt0, doLbl, contLbl) << Symbols::LF;
            out << doLbl << ":" << Symbols::LF;
            const std::string lastIndex = nextTemp();
            out << std::format("  {} = add i64 {}, -1", lastIndex, len) << Symbols::LF;
            const std::string charPtr = nextTemp();
            out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", charPtr, pstr, lastIndex) << Symbols::LF;
            const std::string charByte = nextTemp();
            out << std::format("  {} = load i8, ptr {}", charByte, charPtr) << Symbols::LF;
            const std::string islf = nextTemp();
            out << std::format("  {} = icmp eq i8 {}, 10", islf, charByte) << Symbols::LF;
            const std::string endLbl = std::format("{}_li_end_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", islf, endLbl, contLbl) << Symbols::LF;
            out << endLbl << ":" << Symbols::LF;
            out << std::format("  store i8 0, ptr {}", charPtr) << Symbols::LF;
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << contLbl << ":" << Symbols::LF;
            const std::string safe = nextTemp();
            out << std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, pstr) << Symbols::LF;
            pstr = safe;
        }
        if (!pstr.empty()) {
            const std::string fmtS = nextTemp();
            out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_str_sp, i64 0", fmtS) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmtS, pstr) << Symbols::LF;
        }
    }
    for (const auto &vname: ins->variables) {
        ensureVarAllocated(out, vname);
        const std::string fmt = nextTemp();
        out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt) << Symbols::LF;
        const std::string tmp = nextTemp(); out << std::format("  {} = alloca double", tmp) << Symbols::LF;
        const std::string scanfCount = nextTemp(); out << std::format("  {} = call i32 (ptr, ...) @scanf(ptr {}, ptr {})", scanfCount, fmt, tmp) << Symbols::LF;
        (void)scanfCount;
        const std::string val = nextTemp(); out << std::format("  {} = load double, ptr {}", val, tmp) << Symbols::LF;
        storeNumberToVar(out, vname, val);
    }
}
// NOLINTEND(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length)

} // namespace gwbasic
