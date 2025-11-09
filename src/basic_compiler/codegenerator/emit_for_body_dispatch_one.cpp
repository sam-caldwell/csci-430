// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForBodyStatement
 * Purpose: Handle a single FOR-body statement; return true if terminates.
 */
bool CodeGenerator::emitForBodyStatement(std::ostringstream& out,
                                         const Stmt* stmt,
                                         const std::string& currLineLabel,
                                         int& localCounter) {
    if (const auto* assignStmt = dyn_cast<AssignStmt>(stmt)) { emitForHandleAssign(out, assignStmt, currLineLabel); return false; }
    if (const auto* printStmt = dyn_cast<PrintStmt>(stmt)) { emitForHandlePrint(out, printStmt, currLineLabel, localCounter); return false; }
    if (const auto* midAssign = dyn_cast<MidAssignStmt>(stmt)) { emitForHandleMidAssign(out, midAssign, currLineLabel, localCounter); return false; }
    if (const auto* onGoto = dyn_cast<OnGotoStmt>(stmt)) { emitForHandleOnGoto(out, onGoto, currLineLabel, localCounter); return false; }
    if (const auto* onGosub = dyn_cast<OnGosubStmt>(stmt)) { emitForHandleOnGosub(out, onGosub, currLineLabel, localCounter); return false; }
    if (const auto* gotoStmt = dyn_cast<GotoStmt>(stmt)) { return emitForHandleGoto(out, gotoStmt); }
    if (const auto* gosubStmt = dyn_cast<GosubStmt>(stmt)) { emitForHandleGosub(out, gosubStmt, currLineLabel, localCounter); return false; }
    if (const auto* arrAssign = dyn_cast<ArrayAssignStmt>(stmt)) { emitForHandleArrayAssign(out, arrAssign, currLineLabel, localCounter); return false; }
    if (isa<StopStmt>(stmt)) { emitForHandleStop(out); return true; }
    if (isa<SystemStmt>(stmt)) { emitForHandleSystem(out); return true; }
    if (const auto* whileStmt = dyn_cast<WhileStmt>(stmt)) { emitWhile(out, whileStmt, currLineLabel, localCounter); return false; }
    if (const auto* ifBlock = dyn_cast<IfBlockStmt>(stmt)) { emitIfBlock(out, ifBlock, currLineLabel, localCounter); return false; }
    if (const auto* forStmt = dyn_cast<ForStmt>(stmt)) { emitFor(out, forStmt, currLineLabel, localCounter); return false; }
    throw CodeGenError("Unsupported statement in FOR body");
}

} // namespace gwbasic
