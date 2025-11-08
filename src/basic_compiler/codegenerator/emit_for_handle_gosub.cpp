// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/GosubStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForHandleGosub
 * Purpose: Emit inline GOSUB inside a FOR body.
 */
void CodeGenerator::emitForHandleGosub(std::ostringstream& out, const GosubStmt* gs, const std::string& currLineLabel, int& localCounter) {
    std::string contLbl = currLineLabel + std::string("_gosub_cont") + std::to_string(++localCounter);
    std::string entryLbl = currLineLabel + std::string("_gosub_entry") + std::to_string(localCounter);
    out << "  br label %" << entryLbl << Symbols::LF;
    emitSubroutineInline(out, gs->targetLine, entryLbl, contLbl);
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic

