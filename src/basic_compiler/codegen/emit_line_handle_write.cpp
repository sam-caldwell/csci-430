// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

namespace gwbasic {

// WRITE handler (minimal: behave like PRINT with separators)
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
void CodeGenerator::emitLineHandleWrite(std::ostringstream &out,
                                        const Stmt *stmt,
                                        const std::string &currLineLabel,
                                        int &localCounter) {
    const auto *writeStmt = dyn_cast<WriteStmt>(stmt);
    if (writeStmt == nullptr) { return; }
    std::vector<const Expr*> items; items.reserve(writeStmt->items.size());
    for (const auto &elem : writeStmt->items) { items.push_back(elem.get()); }
    for (size_t pi = 0; pi < items.size(); ++pi) {
        const bool last = (pi + 1 == items.size());
        const Expr *itemExpr = items[pi];
        if (isStringExpr(itemExpr)) {
            const std::string sptr = emitExpr(out, itemExpr, "");
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmt, (last ? "@.fmt_str" : "@.fmt_str_sp")) << Symbols::LF;
            if (writeStmt->channel >= 1) {
                const std::string fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, writeStmt->channel - 1) << Symbols::LF;
                const std::string fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, fptr) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fileHandle, fmt, sptr) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, sptr) << Symbols::LF;
            }
        } else {
            const std::string val = emitExpr(out, itemExpr, "");
            const std::string fmtF = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtF, (last ? "@.fmt_num" : "@.fmt_num_sp")) << Symbols::LF;
            const std::string fmtI = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", fmtI, (last ? "@.fmt_int" : "@.fmt_int_sp")) << Symbols::LF;
            const std::string intVal = nextTemp(); out << std::format("  {} = fptosi double {} to i64", intVal, val) << Symbols::LF;
            const std::string dblVal = nextTemp(); out << std::format("  {} = sitofp i64 {} to double", dblVal, intVal) << Symbols::LF;
            const std::string isInt = nextTemp(); out << std::format("  {} = fcmp oeq double {}, {}", isInt, dblVal, val) << Symbols::LF;
            const std::string intLbl = std::format("{}_w_i_{}", currLineLabel, ++localCounter);
            const std::string floatLbl = std::format("{}_w_f_{}", currLineLabel, localCounter);
            const std::string contLbl = std::format("{}_w_c_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", isInt, intLbl, floatLbl) << Symbols::LF;
            out << intLbl << ":" << Symbols::LF;
            if (writeStmt->channel >= 1) {
                const std::string fptr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, writeStmt->channel - 1) << Symbols::LF;
                const std::string fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, fptr) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i64 {})", fileHandle, fmtI, intVal) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmtI, intVal) << Symbols::LF;
            }
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << floatLbl << ":" << Symbols::LF;
            if (writeStmt->channel >= 1) {
                const std::string fptr2 = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr2, writeStmt->channel - 1) << Symbols::LF;
                const std::string fileHandle2 = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle2, fptr2) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, double {})", fileHandle2, fmtF, val) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, double {})", fmtF, val) << Symbols::LF;
            }
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << contLbl << ":" << Symbols::LF;
        }
    }
    (void)currLineLabel; (void)localCounter;
}

} // namespace gwbasic
