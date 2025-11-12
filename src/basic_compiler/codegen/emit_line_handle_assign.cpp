// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/AssignStmt.h"
#include <sstream>
#include <string>
#include <format>

namespace gwbasic {

void CodeGenerator::emitLineHandleAssign(std::ostringstream &out, const AssignStmt *asg) {
    const std::string val = emitExpr(out, asg->value.get(), "");
    if (isStringVarNameCG(asg->name)) {
        const std::string ir = std::format("  store ptr {}, ptr {}", val, varAllocaName_[asg->name]);
        out << ir << Symbols::LF;
        { std::ostringstream m; m << "line " << currentLine_ << ' ' << nodeName(asg) << " -> " << ir; log() << m.str() << Symbols::LF; }
    } else {
        storeNumberToVar(out, asg->name, val);
    }
}

} // namespace gwbasic
