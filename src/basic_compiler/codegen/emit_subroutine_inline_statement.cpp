// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitSubroutineInlineStatement
 * Summary: Dispatch and emit a single statement within an inlined subroutine.
 * Parameters:
 *  - out: IR output stream.
 *  - stmt: Statement node.
 *  - entryLabel: Base label for naming blocks within this subroutine.
 *  - returnLabel: Continuation label after subroutine returns.
 *  - localCounter: Per-line counter for uniquifying labels.
 * Returns:
 *  - bool: true if control flow terminates (GOTO/RETURN/END), else false.
 */
bool CodeGenerator::emitSubroutineInlineStatement(std::ostringstream& out,
                                                  const Stmt* stmt,
                                                  std::string_view entryLabel,
                                                  const std::string& returnLabel,
                                                  int& localCounter) {
    if (const auto* assignStmt = dyn_cast<AssignStmt>(stmt)) {
        emitSubHandleAssign(out, assignStmt, entryLabel);
        return false;
    }
    if (const auto* midAssign = dyn_cast<MidAssignStmt>(stmt)) {
        emitSubHandleMidAssign(out, midAssign, entryLabel, localCounter);
        return false;
    }
    if (const auto* printStmt = dyn_cast<PrintStmt>(stmt)) {
        emitSubHandlePrint(out, printStmt, entryLabel, localCounter);
        return false;
    }
    if (const auto* inputStmt = dyn_cast<InputStmt>(stmt)) {
        emitSubHandleInput(out, inputStmt, entryLabel);
        return false;
    }
    if (const auto* ifStmt = dyn_cast<IfStmt>(stmt)) {
        emitSubHandleIf(out, ifStmt, entryLabel, localCounter);
        return false;
    }
    if (const auto* gotoStmt = dyn_cast<GotoStmt>(stmt)) {
        const std::string irLine = std::format("  br label %{}", lineLabelName(gotoStmt->targetLine));
        out << irLine << Symbols::LF;
        log() << "line " << currentLine_ << " GotoStmt -> " << irLine << Symbols::LF;
        return true;
    }
    if (const auto* gosubStmt = dyn_cast<GosubStmt>(stmt)) {
        emitSubHandleGosub(out, gosubStmt, entryLabel, localCounter);
        return false;
    }
    if (const auto* forStmt = dyn_cast<ForStmt>(stmt)) {
        emitSubHandleFor(out, forStmt, entryLabel, localCounter);
        return false;
    }
    if (isa<ReturnStmt>(stmt)) {
        const std::string irLine = std::format("  br label %{}", returnLabel);
        out << irLine << Symbols::LF;
        log() << "line " << currentLine_ << " ReturnStmt -> " << irLine << Symbols::LF;
        return true;
    }
    if (isa<EndStmt>(stmt)) {
        const std::string irLine = std::format("  br label %exit");
        out << irLine << Symbols::LF;
        log() << "line " << currentLine_ << " EndStmt -> " << irLine << Symbols::LF;
        return true;
    }
    throw CodeGenError("Unsupported statement in GOSUB body");
}

} // namespace gwbasic
