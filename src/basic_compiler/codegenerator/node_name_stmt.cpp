// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic {

const char* CodeGenerator::nodeName(const Stmt* s) {
    return prettyName(s ? s->getKind() : NodeKind::AbstractStmt);
}

} // namespace gwbasic
