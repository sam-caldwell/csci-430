// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

// WRITE handler (minimal: behave like PRINT with separators)
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitLineHandleWrite(std::ostringstream &out,
                                        const Stmt *stmt,
                                        const std::string &currLineLabel,
                                        int &localCounter) {
    const auto *wr = dyn_cast<WriteStmt>(stmt);
    if (!wr) { return; }
    std::vector<const Expr*> items; items.reserve(wr->items.size());
    for (const auto &elem : wr->items) { items.push_back(elem.get()); }
    for (size_t pi = 0; pi < items.size(); ++pi) {
        const bool last = (pi + 1 == items.size());
        const Expr *v = items[pi];
        if (isStringExpr(v)) {
            const std::string sptr = emitExpr(out, v, "");
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")) << Symbols::LF;
            if (wr->channel >= 1) {
                const std::string fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, wr->channel - 1) << Symbols::LF;
                const std::string fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fh, fmt, sptr) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr) << Symbols::LF;
            }
        } else {
            const std::string val = emitExpr(out, v, "");
            const std::string fmtF = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, (last ? "@.fmt_num" : "@.fmt_num_sp")) << Symbols::LF;
            const std::string fmtI = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, (last ? "@.fmt_int" : "@.fmt_int_sp")) << Symbols::LF;
            const std::string iv = nextTemp(); out << std::format("  {} = fptosi double {} to i64", iv, val) << Symbols::LF;
            const std::string dv = nextTemp(); out << std::format("  {} = sitofp i64 {} to double", dv, iv) << Symbols::LF;
            const std::string isInt = nextTemp(); out << std::format("  {} = fcmp oeq double {}, {}", isInt, dv, val) << Symbols::LF;
            const std::string il = std::format("{}_w_i_{}", currLineLabel, ++localCounter);
            const std::string fl = std::format("{}_w_f_{}", currLineLabel, localCounter);
            const std::string cl = std::format("{}_w_c_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", isInt, il, fl) << Symbols::LF;
            out << il << ":" << Symbols::LF;
            if (wr->channel >= 1) {
                const std::string fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, wr->channel - 1) << Symbols::LF;
                const std::string fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fh, fmtI, iv) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, iv) << Symbols::LF;
            }
            out << std::format("  br label %{}", cl) << Symbols::LF;
            out << fl << ":" << Symbols::LF;
            if (wr->channel >= 1) {
                const std::string fptr2 = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr2, wr->channel - 1) << Symbols::LF;
                const std::string fh2 = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh2, fptr2) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fh2, fmtF, val) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val) << Symbols::LF;
            }
            out << std::format("  br label %{}", cl) << Symbols::LF;
            out << cl << ":" << Symbols::LF;
        }
    }
    (void)currLineLabel; (void)localCounter;
}

} // namespace gwbasic
