// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ForStmt.h"
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitForBodyStatements
 * Summary: Dispatch and emit each statement inside a FOR loop body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - fs: Enclosing ForStmt (provides body vector).
 *  - currLineLabel: Label stem for nested labels.
 *  - localCounter: Counter for uniquifying labels.
 * Returns:
 *  - bool: True when the body terminates control flow (e.g., GOTO).
 */
bool CodeGenerator::emitForBodyStatements(std::ostringstream& out,
                                          const ForStmt* forStmt,
                                          const std::string& currLineLabel,
                                          int& localCounter) {
    for (const auto& stmtNode : forStmt->body) {
        if (emitForBodyStatement(out, stmtNode.get(), currLineLabel, localCounter)) {
            return true;
        }
    }
    return false;
}

} // namespace gwbasic
