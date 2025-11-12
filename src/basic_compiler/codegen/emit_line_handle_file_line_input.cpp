// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/FileInputStmt.h"
#include "basic_compiler/ast/LineInputStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// INPUT #n and LINE INPUT [#n]
// NOLINTNEXTLINE(readability-function-size,readability-identifier-length)
void CodeGenerator::emitLineHandleFileLineInput(std::ostringstream &out,
                                                const Stmt *stmt,
                                                const std::string &currLineLabel,
                                                int &localCounter) {
    if (const auto *fi = dyn_cast<FileInputStmt>(stmt)) {
        const int idx = fi->channel - 1;
        const std::string ep = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, idx) << Symbols::LF;
        const std::string fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, ep) << Symbols::LF;
        for (const auto &vname : fi->variables) {
            ensureVarAllocated(out, vname);
            const std::string tmp = nextTemp(); out << std::format("  {} = alloca double", tmp) << Symbols::LF;
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @fscanf(ptr {}, ptr {}, ptr {})", fh, fmt, tmp) << Symbols::LF;
            const std::string dv = nextTemp(); out << std::format("  {} = load double, ptr {}", dv, tmp) << Symbols::LF;
            storeNumberToVar(out, vname, dv);
        }
        return;
    }
    if (const auto *li = dyn_cast<LineInputStmt>(stmt)) {
        const std::string buf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_lbuf, i64 0, i64 0", buf) << Symbols::LF;
        if (li->channel == 0) {
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_line_in, i64 0", fmt) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, buf) << Symbols::LF;
        } else {
            const int idx = li->channel - 1;
            const std::string ep = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, idx) << Symbols::LF;
            const std::string fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, ep) << Symbols::LF;
            out << std::format("  call ptr @fgets(ptr {}, i32 256, ptr {})", buf, fh) << Symbols::LF;
            const std::string len = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", len, buf) << Symbols::LF;
            const std::string gt0 = nextTemp(); out << std::format("  {} = icmp sgt i64 {}, 0", gt0, len) << Symbols::LF;
            const std::string doLbl = std::format("{}_li_do_{}", currLineLabel, ++localCounter);
            const std::string contLbl = std::format("{}_li_cont_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", gt0, doLbl, contLbl) << Symbols::LF;
            out << doLbl << ":" << Symbols::LF;
            const std::string m1 = nextTemp(); out << std::format("  {} = add i64 {}, -1", m1, len) << Symbols::LF;
            const std::string pch = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pch, buf, m1) << Symbols::LF;
            const std::string ch = nextTemp(); out << std::format("  {} = load i8, ptr {}", ch, pch) << Symbols::LF;
            const std::string islf = nextTemp(); out << std::format("  {} = icmp eq i8 {}, 10", islf, ch) << Symbols::LF;
            const std::string endLbl = std::format("{}_li_end_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", islf, endLbl, contLbl) << Symbols::LF;
            out << endLbl << ":" << Symbols::LF;
            out << std::format("  store i8 0, ptr {}", pch) << Symbols::LF;
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << contLbl << ":" << Symbols::LF;
        }
        const std::string n = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", n, buf) << Symbols::LF;
        const std::string size = nextTemp(); out << std::format("  {} = add i64 {}, 1", size, n) << Symbols::LF;
        const std::string mem = nextTemp(); out << std::format("  {} = call ptr @malloc(i64 {})", mem, size) << Symbols::LF;
        out << std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", mem, buf, n) << Symbols::LF;
        const std::string pn = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", pn, mem, n) << Symbols::LF;
        out << std::format("  store i8 0, ptr {}", pn) << Symbols::LF;
        ensureVarAllocated(out, li->name);
        out << std::format("  store ptr {}, ptr {}", mem, varAllocaName_[li->name]) << Symbols::LF;
        (void)currLineLabel; (void)localCounter;
    }
}

} // namespace gwbasic
