// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// ON GOTO / ON GOSUB grouped handler (does not terminate line by itself)
// NOLINTNEXTLINE(readability-function-size,readability-identifier-length)
void CodeGenerator::emitLineHandleOnDispatch(std::ostringstream &out,
                                             const Stmt *stmt,
                                             const std::string &currLineLabel,
                                             int &localCounter) {
    if (const auto *og = dyn_cast<OnGotoStmt>(stmt)) {
        const std::string idx = emitExpr(out, og->index.get(), currLineLabel);
        const std::string idxi32 = nextTemp();
        out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
        const std::string contLbl = std::format("{}_on_cont_{}", currLineLabel, ++localCounter);
        std::ostringstream ir;
        ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
        for (size_t i = 0; i < og->targets.size(); ++i) {
            ir << " i32 " << (i + 1) << ", label %" << lineLabelName(og->targets[i]);
        }
        ir << " ]";
        out << ir.str() << Symbols::LF;
        log() << "line " << currentLine_ << " OnGoto switch -> " << ir.str() << Symbols::LF;
        out << contLbl << ":" << Symbols::LF;
        return;
    }
    if (const auto *ogs = dyn_cast<OnGosubStmt>(stmt)) {
        const std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
        const std::string idxi32 = nextTemp();
        out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
        const std::string contLbl = std::format("{}_on_gs_cont_{}", currLineLabel, ++localCounter);
        std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
        for (size_t i = 0; i < ogs->targets.size(); ++i) {
            entryLbls.emplace_back(std::format("{}_on_gs_entry_{}_{}", currLineLabel, localCounter, i + 1));
        }
        std::ostringstream ir;
        ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
        for (size_t i = 0; i < ogs->targets.size(); ++i) {
            ir << " i32 " << (i + 1) << ", label %" << entryLbls[i];
        }
        ir << " ]";
        out << ir.str() << Symbols::LF;
        log() << "line " << currentLine_ << " OnGosub switch -> " << ir.str() << Symbols::LF;
        for (size_t i = 0; i < ogs->targets.size(); ++i) {
            emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
        }
        out << contLbl << ":" << Symbols::LF;
    }
}

} // namespace gwbasic
