// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ErrorStmt.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ResumeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// ON ERROR GOTO / ERROR n / RESUME*
// Returns true if control branches away and the line should be considered terminated.
// NOLINTNEXTLINE(readability-function-size,readability-identifier-length)
bool CodeGenerator::emitLineHandleErrorHandlers(std::ostringstream &out,
                                                const Stmt *stmt,
                                                int stmtIndex) {
    if (const auto *oeg = dyn_cast<OnErrorGotoStmt>(stmt)) {
        const std::string branchIr = std::format("  store i32 {}, ptr @gwb_err_trap_line", oeg->targetLine);
        out << branchIr << Symbols::LF;
        logLine(std::string("OnErrorGoto trap -> ") + branchIr);
        return false;
    }
    if (const auto *ers = dyn_cast<ErrorStmt>(stmt)) {
        const std::string doubleVal = emitExpr(out, ers->code.get(), "");
        const std::string i32v = nextTemp();
        out << std::format("  {} = fptosi double {} to i32", i32v, doubleVal) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_err_code", i32v) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_err_line", currentLine_) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_resume_line", currentLine_) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
        out << "  store i1 true, ptr @gwb_in_handler" << Symbols::LF;
        const std::string trap = nextTemp();
        out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
        out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
        for (int lnum : lineNumbers_) {
            out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF;
        }
        out << "  ]" << Symbols::LF;
        return true;
    }
    if (const auto *resumeStmt = dyn_cast<ResumeStmt>(stmt)) {
        if (resumeStmt->kind == ResumeStmt::Kind::Line) {
            const std::string branchIr = std::format("  br label %{}", lineLabelName(resumeStmt->line));
            out << branchIr << Symbols::LF;
            logLine(std::string("Resume line -> ") + branchIr);
            return true;
        }
        if (resumeStmt->kind == ResumeStmt::Kind::Reexecute) {
            const std::string branchIr = std::format("  br label %{}", resumeLabelName(currentLine_, stmtIndex));
            out << branchIr << Symbols::LF;
            logLine(std::string("Resume reexec -> ") + branchIr);
            return true;
        }
        // Resume next
        const std::string branchIr = std::format("  br label %{}", resumeNextLabelName(currentLine_, stmtIndex));
        out << branchIr << Symbols::LF;
        logLine(std::string("Resume next -> ") + branchIr);
        return true;
    }
    (void)stmtIndex; // unused in some code paths
    return false;
}

} // namespace gwbasic
