// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <algorithm>
#include <format>
#include <numeric>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitMainPrologue
 * Summary: Emit function prologue, locals, and initial branch.
 * Parameters:
 *  - out: IR output stream to append to.
 * Returns:
 *  - void
 */
void CodeGenerator::emitMainPrologue(std::ostringstream& out) {
    out << "define i32 @main() {" << Symbols::LF
        << "entry:" << Symbols::LF;
    // Hoist all local allocations (scalars/arrays) to the entry block so that
    // allocas dominate all uses in the IR. This prevents invalid IR like
    // "Instruction does not dominate all uses!" seen under lli and avoids
    // undefined behavior at runtime.
    // 1) Scalars discovered during declaration collection
    for (const auto& varName : variables_) {
        ensureVarAllocated(out, varName);
    }
    // Ensure error reporting temporaries always exist; these may be referenced
    // by bounds/range checks even if the program never explicitly declares them.
    ensureVarAllocated(out, "ERR");
    ensureVarAllocated(out, "ERL");
    // 2) Arrays: allocate backing storage for all arrays discovered by semantics
    //    using their declared extents and current OPTION BASE.
    for (const auto& [name, dims] : arrayDims_) {
        const long long total = std::accumulate(
            dims.begin(), dims.end(), 1LL,
            [this](long long acc, int upperBound) {
                const long long extent = std::max<long long>(static_cast<long long>(upperBound) - optionBase_ + 1, 0LL);
                return acc * extent;
            });
        if (isStringArrayNameCG(name)) {
            ensureStringArrayAllocated(out, name, static_cast<int>(total));
        } else {
            ensureArrayAllocated(out, name, static_cast<int>(total));
        }
    }
    if (!lineNumbers_.empty()) {
        const std::string branchInstr = std::format("  br label %{}", lineLabelName(lineNumbers_.front()));
        out << branchInstr << Symbols::LF;
        log() << "entry -> " << branchInstr << Symbols::LF;
    } else {
        out << "  ret i32 0" << Symbols::LF;
        out << "}" << Symbols::LF;
    }
}

} // namespace gwbasic
