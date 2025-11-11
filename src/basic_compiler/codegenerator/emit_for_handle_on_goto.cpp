// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <cstddef>
#include <format>
#include <sstream>
#include <string>

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
void CodeGenerator::emitForHandleOnGoto(std::ostringstream& out, const OnGotoStmt* on_goto, const std::string& currLineLabel, int& localCounter) {
    const std::string idx = emitExpr(out, on_goto->index.get(), currLineLabel);
    const std::string idxi32 = nextTemp(); out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
    const std::string contLbl = std::format("{}_on_cont_{}", currLineLabel, ++localCounter);
    out << std::format("  switch i32 {}, label %{} [", idxi32, contLbl) << Symbols::LF;
    for (size_t i = 0; i < on_goto->targets.size(); ++i) {
        out << std::format("    i32 {}, label %{}", i + 1, lineLabelName(on_goto->targets[i])) << Symbols::LF;
    }
    out << "  ]" << Symbols::LF
        << std::format("{}:", contLbl) << Symbols::LF;
}

} // namespace gwbasic
