// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// Handle simple flow-altering statements. Returns true if line terminates.
// NOLINTNEXTLINE(readability-function-size,readability-identifier-length)
bool CodeGenerator::emitLineHandleBranching(std::ostringstream &out,
                                            const Stmt *stmt,
                                            const std::string &currLineLabel,
                                            int &localCounter) {
    if (const auto *gt = dyn_cast<GotoStmt>(stmt)) {
        const std::string ir = std::format("  br label %{}", lineLabelName(gt->targetLine));
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " GotoStmt -> " << ir << Symbols::LF;
        return true;
    }
    if (const auto *gs = dyn_cast<GosubStmt>(stmt)) {
        const std::string contLbl = std::format("{}_cont{}", currLineLabel, ++localCounter);
        const std::string entryLbl = std::format("{}_gosub_entry{}", currLineLabel, localCounter);
        out << std::format("  br label %{}", entryLbl) << Symbols::LF;
        emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
        out << contLbl << ":" << Symbols::LF;
        return false;
    }
    if (const auto *is = dyn_cast<IfStmt>(stmt)) {
        const auto *be = dyn_cast<BinaryExpr>(is->cond.get());
        if (!be || (be->op != BinaryOp::Eq && be->op != BinaryOp::Ne && be->op != BinaryOp::Lt &&
                    be->op != BinaryOp::Le && be->op != BinaryOp::Gt && be->op != BinaryOp::Ge)) {
            throw CodeGenError("IF condition must be a comparison");
        }
        const std::string cond = emitComparison(out, be);
        const std::string contLbl = std::format("{}_cont{}", currLineLabel, ++localCounter);
        const std::string ir = std::format("  br i1 {}, label %{}, label %{}", cond, lineLabelName(is->targetLine), contLbl);
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " IfStmt -> " << ir << Symbols::LF;
        out << contLbl << ":" << Symbols::LF;
        return false;
    }
    if (isa<ReturnStmt>(stmt)) {
        const std::string ir = std::format("  br label %exit");
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " ReturnStmt -> " << ir << Symbols::LF;
        return true;
    }
    if (isa<EndStmt>(stmt)) {
        const std::string ir = std::format("  br label %exit");
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " EndStmt -> " << ir << Symbols::LF;
        return true;
    }
    if (isa<StopStmt>(stmt)) {
        // STOP: print break message and terminate
        const std::string fmt = nextTemp();
        out << std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_) << Symbols::LF;
        out << "  br label %exit" << Symbols::LF;
        log() << "line " << currentLine_ << " StopStmt -> break+exit" << Symbols::LF;
        return true;
    }
    if (isa<SystemStmt>(stmt)) {
        const std::string ir = std::format("  br label %exit");
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " SystemStmt -> " << ir << Symbols::LF;
        return true;
    }
    (void)currLineLabel; (void)localCounter; // silence unused for some builds
    return false;
}

} // namespace gwbasic
