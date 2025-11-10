// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/AssignStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: emitForHandleAssign
 * Summary: Emit IR for an assignment inside a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - asg: Parsed AssignStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleAssign(std::ostringstream& out, const AssignStmt* asg, const std::string& currLineLabel) {
    (void)currLineLabel; // label isn't needed here
    std::string val = emitExpr(out, asg->value.get(), currLineLabel);
    if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
        out << std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]) << Symbols::LF;
    } else {
        storeNumberToVar(out, asg->name, val);
    }
}

} // namespace gwbasic
