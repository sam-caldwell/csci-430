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
        if (emitForBodyStatement(out, s.get(), currLineLabel, localCounter)) return true;
    }
    return false;
}

} // namespace gwbasic
