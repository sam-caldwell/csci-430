// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/codegen/CodeGenError.h"
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
    if (const auto *gotoStmt = dyn_cast<GotoStmt>(stmt)) {
        const std::string branchIr = std::format("  br label %{}", lineLabelName(gotoStmt->targetLine));
        out << branchIr << Symbols::LF;
        return true;
    }
    if (const auto *gosubStmt = dyn_cast<GosubStmt>(stmt)) {
        const std::string contLbl = std::format("{}_cont{}", currLineLabel, ++localCounter);
        const std::string entryLbl = std::format("{}_gosub_entry{}", currLineLabel, localCounter);
        out << std::format("  br label %{}", entryLbl) << Symbols::LF;
        emitSubroutineInline(out, gosubStmt->targetLine, entryLbl, contLbl);
        out << contLbl << ":" << Symbols::LF;
        return false;
    }
    if (const auto *ifStmt = dyn_cast<IfStmt>(stmt)) {
        const auto *binExpr = dyn_cast<BinaryExpr>(ifStmt->cond.get());
        if (binExpr == nullptr || (binExpr->op != BinaryOp::Eq && binExpr->op != BinaryOp::Ne && binExpr->op != BinaryOp::Lt &&
                                   binExpr->op != BinaryOp::Le && binExpr->op != BinaryOp::Gt && binExpr->op != BinaryOp::Ge)) {
            throw CodeGenError("IF condition must be a comparison");
        }
        const std::string cond = emitComparison(out, binExpr);
        const std::string contLbl = std::format("{}_cont{}", currLineLabel, ++localCounter);
        const std::string branchIr = std::format("  br i1 {}, label %{}, label %{}", cond, lineLabelName(ifStmt->targetLine), contLbl);
        out << branchIr << Symbols::LF;
        out << contLbl << ":" << Symbols::LF;
        return false;
    }
    if (isa<ReturnStmt>(stmt)) {
        const std::string branchIr = std::format("  br label %exit");
        out << branchIr << Symbols::LF;
        return true;
    }
    if (isa<EndStmt>(stmt)) {
        const std::string branchIr = std::format("  br label %exit");
        out << branchIr << Symbols::LF;
        return true;
    }
    if (isa<StopStmt>(stmt)) {
        // STOP: print break message and terminate
        const std::string fmt = nextTemp();
        out << std::format("  {} = getelementptr inbounds i8, ptr @.msg_break, i64 0", fmt) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {})", fmt, currentLine_) << Symbols::LF;
        out << "  br label %exit" << Symbols::LF;
        return true;
    }
    if (isa<SystemStmt>(stmt)) {
        const std::string branchIr = std::format("  br label %exit");
        out << branchIr << Symbols::LF;
        return true;
    }
    (void)currLineLabel; (void)localCounter; // silence unused for some builds
    return false;
}

} // namespace gwbasic
