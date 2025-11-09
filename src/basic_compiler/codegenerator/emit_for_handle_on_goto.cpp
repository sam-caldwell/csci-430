// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: emitForHandleOnGoto
 * Summary: Emit IR for ON ... GOTO inside a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - og: Parsed OnGotoStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleOnGoto(std::ostringstream& out, const OnGotoStmt* og, const std::string& currLineLabel, int& localCounter) {
    std::string idx = emitExpr(out, og->index.get(), currLineLabel);
    std::string idxi32 = nextTemp(); out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
    std::string contLbl = currLineLabel + std::string("_on_cont_") + std::to_string(++localCounter);
    std::ostringstream ir; ir << "  switch i32 " << idxi32 << ", label %" << contLbl << " [";
    for (size_t i = 0; i < og->targets.size(); ++i) ir << " i32 " << (i + 1) << ", label %" << lineLabelName(og->targets[i]);
    ir << " ]";
    out << ir.str() << Symbols::LF;
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
