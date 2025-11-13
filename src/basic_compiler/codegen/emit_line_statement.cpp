// (c) 2025 Sam Caldwell. All Rights Reserved.
// NOLINTBEGIN(llvm-include-order,misc-include-cleaner)
#include "basic_compiler/codegen/CodeGenerator.h"

#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/RestoreStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/CloseStmt.h"
#include "basic_compiler/ast/WidthStmt.h"
#include "basic_compiler/ast/FileInputStmt.h"
#include "basic_compiler/ast/LineInputStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/ErrorStmt.h"
#include "basic_compiler/ast/ResumeStmt.h"
#include "basic_compiler/ast/ClsStmt.h"
#include "basic_compiler/ast/LocateStmt.h"
#include "basic_compiler/ast/ListStmt.h"
#include "basic_compiler/ast/FilesStmt.h"
#include "basic_compiler/ast/MkdirStmt.h"
#include "basic_compiler/ast/RmdirStmt.h"
#include "basic_compiler/ast/KillStmt.h"
#include "basic_compiler/ast/NameStmt.h"
#include "basic_compiler/ast/ShellStmt.h"
#include "basic_compiler/ast/EnvironStmt.h"
#include "basic_compiler/ast/BeepStmt.h"
#include "basic_compiler/ast/ChdirStmt.h"
#include "basic_compiler/ast/ClearStmt.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/ScreenStmt.h"
#include "basic_compiler/ast/CircleStmt.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/UnsupportedStmt.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/DefUsrStmt.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/BsaveStmt.h"
#include "basic_compiler/ast/PokeStmt.h"
#include "basic_compiler/ast/CallAbsStmt.h"

#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitLineStatement
 * Summary: Dispatch a single line-context statement to the appropriate helper.
 * Returns true when control flow for the line terminates in this statement.
 */
// NOLINTNEXTLINE(readability-identifier-length)
bool CodeGenerator::emitLineStatement(std::ostringstream &out,
                                      const Stmt *stmt,
                                      const std::string &currLineLabel,
                                      int &localCounter,
                                      const std::string &nextLabel,
                                      int stmtIndex) {
    // Fast-path common kinds first
    if (const auto *asg = dyn_cast<AssignStmt>(stmt)) {
        emitLineHandleAssign(out, asg);
        return false;
    }
    if (const auto *mid = dyn_cast<MidAssignStmt>(stmt)) {
        emitLineHandleMidAssign(out, mid, currLineLabel, localCounter, stmtIndex, currentLine_);
        return false;
    }
    if (const auto *arr = dyn_cast<ArrayAssignStmt>(stmt)) {
        emitLineHandleArrayAssign(out, arr, stmtIndex, currLineLabel, localCounter);
        return false;
    }
    if (const auto *printStmt = dyn_cast<PrintStmt>(stmt)) {
        emitLineHandlePrint(out, printStmt, currLineLabel, localCounter);
        return false;
    }
    if (const auto *ins = dyn_cast<InputStmt>(stmt)) {
        emitLineHandleInput(out, ins, currLineLabel, localCounter);
        return false;
    }

    // Grouped handlers
    if (emitLineHandleBranching(out, stmt, currLineLabel, localCounter)) {
        return true;
    }
    emitLineHandleOnDispatch(out, stmt, currLineLabel, localCounter);

    if (emitLineHandleRunChainMerge(out, stmt)) {
        return true;
    }

    if (emitLineHandleErrorHandlers(out, stmt, stmtIndex)) {
        return true;
    }

    emitLineHandleOpenCloseWidth(out, stmt, currLineLabel, localCounter);
    emitLineHandleFileLineInput(out, stmt, currLineLabel, localCounter);
    emitLineHandleReadRestoreData(out, stmt, stmtIndex, currLineLabel, localCounter);
    emitLineHandleWrite(out, stmt, currLineLabel, localCounter);
    emitLineHandleFsOsEnvConsole(out, stmt, currLineLabel, localCounter, nextLabel);
    emitLineHandleRandomize(out, stmt);
    emitLineHandleSwap(out, stmt, stmtIndex, currLineLabel, localCounter);
    emitLineHandleMiscNoops(out, stmt);

    // Structured blocks retain their dedicated helpers
    if (const auto *ifBlock = dyn_cast<IfBlockStmt>(stmt)) {
        emitIfBlock(out, ifBlock, currLineLabel, localCounter);
        return false;
    }
    if (const auto *whileStmt = dyn_cast<WhileStmt>(stmt)) {
        emitWhile(out, whileStmt, currLineLabel, localCounter);
        return false;
    }
    if (const auto *forStmt = dyn_cast<ForStmt>(stmt)) {
        emitFor(out, forStmt, currLineLabel, localCounter);
        return false;
    }

    return false; // default: not terminating
}

} // namespace gwbasic
// NOLINTEND(llvm-include-order,misc-include-cleaner)
