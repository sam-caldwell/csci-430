// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/GotoStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForHandleGoto
 * Purpose: Emit IR for GOTO inside a FOR body and signal termination.
 */
bool CodeGenerator::emitForHandleGoto(std::ostringstream& out, const GotoStmt* gt) {
    std::string ir = "  br label %"; ir += lineLabelName(gt->targetLine);
    out << ir << Symbols::LF;
    return true;
}

} // namespace gwbasic

