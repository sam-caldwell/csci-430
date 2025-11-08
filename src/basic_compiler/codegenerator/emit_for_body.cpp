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
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForBodyStatements
 * Purpose:
 *  - Dispatch and emit each statement inside a FOR loop body.
 * Inputs:
 *  - out: IR stream to append
 *  - fs: enclosing ForStmt (provides body vector)
 *  - currLineLabel: label stem for nested labels
 *  - localCounter: counter for uniquifying labels
 * Outputs:
 *  - bool: true when the body terminates control flow (e.g., GOTO)
 */
bool CodeGenerator::emitForBodyStatements(std::ostringstream& out,
                                          const ForStmt* fs,
                                          const std::string& currLineLabel,
                                          int& localCounter) {
    for (const auto& s : fs->body) {
        if (auto asg = dyn_cast<AssignStmt>(s.get())) {
            emitForHandleAssign(out, asg, currLineLabel);
            continue;
        }
        if (auto pr = dyn_cast<PrintStmt>(s.get())) {
            emitForHandlePrint(out, pr, currLineLabel, localCounter);
            continue;
        }
        if (auto mid = dyn_cast<MidAssignStmt>(s.get())) {
            emitForHandleMidAssign(out, mid, currLineLabel, localCounter);
            continue;
        }
        if (auto og = dyn_cast<OnGotoStmt>(s.get())) {
            emitForHandleOnGoto(out, og, currLineLabel, localCounter);
            continue;
        }
        if (auto ogs = dyn_cast<OnGosubStmt>(s.get())) {
            emitForHandleOnGosub(out, ogs, currLineLabel, localCounter);
            continue;
        }
        if (auto gt = dyn_cast<GotoStmt>(s.get())) {
            if (emitForHandleGoto(out, gt)) return true; // terminated
            continue;
        }
        if (auto gs = dyn_cast<GosubStmt>(s.get())) {
            emitForHandleGosub(out, gs, currLineLabel, localCounter);
            continue;
        }
        if (auto aaset = dyn_cast<ArrayAssignStmt>(s.get())) {
            emitForHandleArrayAssign(out, aaset, currLineLabel, localCounter);
            continue;
        }
        if (isa<StopStmt>(s.get())) { emitForHandleStop(out); return true; }
        if (isa<SystemStmt>(s.get())) { emitForHandleSystem(out); return true; }
        if (auto w = dyn_cast<WhileStmt>(s.get())) {
            emitWhile(out, w, currLineLabel, localCounter);
            continue;
        }
        if (auto ib = dyn_cast<IfBlockStmt>(s.get())) {
            emitIfBlock(out, ib, currLineLabel, localCounter);
            continue;
        }
        if (auto nf = dyn_cast<ForStmt>(s.get())) {
            emitFor(out, nf, currLineLabel, localCounter);
            continue;
        }
        throw CodeGenError("Unsupported statement in FOR body");
    }
    return false;
}

} // namespace gwbasic
