// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

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
void CodeGenerator::emitForHandleOnGosub(std::ostringstream& out, const OnGosubStmt* on_gosub, const std::string& currLineLabel, int& localCounter) {
    const std::string idx = emitExpr(out, on_gosub->index.get(), currLineLabel);
    const std::string idxi32 = nextTemp();
    out << std::format("  {} = fptosi double {} to i32", idxi32, idx) << Symbols::LF;
    const std::string contLbl = std::format("{}_on_gs_cont_{}", currLineLabel, ++localCounter);
    std::vector<std::string> entryLbls; entryLbls.reserve(on_gosub->targets.size());
    for (size_t i = 0; i < on_gosub->targets.size(); ++i) {
        entryLbls.push_back(std::format("{}_on_gs_entry_{}_{}", currLineLabel, localCounter, i + 1));
    }
    out << std::format("  switch i32 {}, label %{} [", idxi32, contLbl) << Symbols::LF;
    for (size_t i = 0; i < on_gosub->targets.size(); ++i) {
        out << std::format("    i32 {}, label %{}", i + 1, entryLbls[i]) << Symbols::LF;
    }
    out << "  ]" << Symbols::LF;
    for (size_t i = 0; i < on_gosub->targets.size(); ++i) {
        emitSubroutineInline(out, on_gosub->targets[i], entryLbls[i], contLbl);
    }
    out << std::format("{}:", contLbl) << Symbols::LF;
}

} // namespace gwbasic
