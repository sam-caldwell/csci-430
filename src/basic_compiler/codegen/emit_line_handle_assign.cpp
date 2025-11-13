// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/AssignStmt.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

void CodeGenerator::emitLineHandleAssign(std::ostringstream &out, const AssignStmt *asg) {
    const std::string val = emitExpr(out, asg->value.get(), "");
    if (isStringVarNameCG(asg->name)) {
        const std::string irLine = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
        out << irLine << Symbols::LF;
        {
            std::ostringstream msg;
            msg << "line " << currentLine_ << ' ' << nodeName(asg) << " -> " << irLine;
            log() << msg.str() << Symbols::LF;
        }
    } else {
        storeNumberToVar(out, asg->name, val);
    }
}

} // namespace gwbasic
