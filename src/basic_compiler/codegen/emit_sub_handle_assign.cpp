// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/AssignStmt.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitSubHandleAssign
 * Summary: Emit scalar assignment inside a subroutine context.
 * Parameters:
 *  - out: IR output stream.
 *  - asg: AssignStmt node.
 *  - entryLabel: Current subroutine entry label (for expression lowering).
 * Returns:
 *  - void
 */
void CodeGenerator::emitSubHandleAssign(std::ostringstream& out, const AssignStmt* asg, std::string_view entryLabel) {
    const std::string val = emitExpr(out, asg->value.get(), std::string(entryLabel));
    if (!asg->name.empty() && asg->name.back() == Symbols::DOLLARSIGN.first()) {
        const std::string irLine = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
        out << irLine << Symbols::LF;
        
        return;
    }
    storeNumberToVar(out, asg->name, val);
}

} // namespace gwbasic
