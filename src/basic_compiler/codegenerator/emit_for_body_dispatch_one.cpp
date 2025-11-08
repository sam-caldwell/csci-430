// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForBodyStatement
 * Purpose: Handle a single FOR-body statement; return true if terminates.
 */
bool CodeGenerator::emitForBodyStatement(std::ostringstream& out,
                                         const Stmt* s,
                                         const std::string& currLineLabel,
                                         int& localCounter) {
    if (auto asg = dyn_cast<AssignStmt>(s)) { emitForHandleAssign(out, asg, currLineLabel); return false; }
    if (auto pr = dyn_cast<PrintStmt>(s)) { emitForHandlePrint(out, pr, currLineLabel, localCounter); return false; }
    if (auto mid = dyn_cast<MidAssignStmt>(s)) { emitForHandleMidAssign(out, mid, currLineLabel, localCounter); return false; }
    if (auto og = dyn_cast<OnGotoStmt>(s)) { emitForHandleOnGoto(out, og, currLineLabel, localCounter); return false; }
    if (auto ogs = dyn_cast<OnGosubStmt>(s)) { emitForHandleOnGosub(out, ogs, currLineLabel, localCounter); return false; }
    if (auto gt = dyn_cast<GotoStmt>(s)) { return emitForHandleGoto(out, gt); }
    if (auto gs = dyn_cast<GosubStmt>(s)) { emitForHandleGosub(out, gs, currLineLabel, localCounter); return false; }
    if (auto aaset = dyn_cast<ArrayAssignStmt>(s)) { emitForHandleArrayAssign(out, aaset, currLineLabel, localCounter); return false; }
    if (isa<StopStmt>(s)) { emitForHandleStop(out); return true; }
    if (isa<SystemStmt>(s)) { emitForHandleSystem(out); return true; }
    if (auto w = dyn_cast<WhileStmt>(s)) { emitWhile(out, w, currLineLabel, localCounter); return false; }
    if (auto ib = dyn_cast<IfBlockStmt>(s)) { emitIfBlock(out, ib, currLineLabel, localCounter); return false; }
    if (auto nf = dyn_cast<ForStmt>(s)) { emitFor(out, nf, currLineLabel, localCounter); return false; }
    throw CodeGenError("Unsupported statement in FOR body");
}

} // namespace gwbasic

