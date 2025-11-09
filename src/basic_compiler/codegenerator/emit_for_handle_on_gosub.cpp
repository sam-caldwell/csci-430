// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: emitForHandleOnGosub
 * Summary: Emit IR for ON ... GOSUB inside a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - ogs: Parsed OnGosubStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleOnGosub(std::ostringstream& out, const OnGosubStmt* ogs, const std::string& currLineLabel, int& localCounter) {
    std::string idx = emitExpr(out, ogs->index.get(), currLineLabel);
    std::string idxi32 = nextTemp(); out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
    std::string contLbl = currLineLabel + std::string("_on_gs_cont_") + std::to_string(++localCounter);
    std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
    for (size_t i = 0; i < ogs->targets.size(); ++i) entryLbls.push_back(currLineLabel + std::string("_on_gs_entry_") + std::to_string(localCounter) + std::string("_") + std::to_string(i + 1));
    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
    for (size_t i = 0; i < ogs->targets.size(); ++i) ir << " i32 " << (i + 1) << ", label %" << entryLbls[i];
    ir << " ]";
    out << ir.str() << Symbols::LF;
    for (size_t i = 0; i < ogs->targets.size(); ++i) emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
