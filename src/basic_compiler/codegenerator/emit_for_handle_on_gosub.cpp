// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include <sstream>
#include <format>

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
    std::string idxi32 = nextTemp();
    out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
    std::string contLbl = std::format("{}_on_gs_cont_{}", currLineLabel, ++localCounter);
    std::vector<std::string> entryLbls; entryLbls.reserve(ogs->targets.size());
    for (size_t i = 0; i < ogs->targets.size(); ++i) {
        entryLbls.push_back(std::format("{}_on_gs_entry_{}_{}", currLineLabel, localCounter, i + 1));
    }
    out << std::format("  switch i32 {}, label %{} [", idxi32, contLbl) << Symbols::LF;
    for (size_t i = 0; i < ogs->targets.size(); ++i) {
        out << std::format("    i32 {}, label %{}", i + 1, entryLbls[i]) << Symbols::LF;
    }
    out << "  ]" << Symbols::LF;
    for (size_t i = 0; i < ogs->targets.size(); ++i) {
        emitSubroutineInline(out, ogs->targets[i], entryLbls[i], contLbl);
    }
    out << std::format("{}:", contLbl) << Symbols::LF;
}

} // namespace gwbasic
