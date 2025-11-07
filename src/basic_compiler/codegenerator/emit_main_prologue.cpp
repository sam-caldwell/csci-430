// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitMainPrologue(std::ostringstream& out) {
    /*
     * Function: CodeGenerator::emitMainPrologue
     * Inputs:
     *  - out: IR output stream
     * Outputs:
     *  - void
     * Theory of operation:
     *  - Starts the main function, allocates all discovered variables on the
     *    stack, initializes them to 0.0, and branches to the first line label
     *    or returns 0 if the program has no lines.
     */
    out << "define i32 @main() {" << Symbols::LF
        << "entry:" << Symbols::LF;
    // Hoist all local allocations (scalars/arrays) to the entry block so that
    // allocas dominate all uses in the IR. This prevents invalid IR like
    // "Instruction does not dominate all uses!" seen under lli and avoids
    // undefined behavior at runtime.
    // 1) Scalars discovered during declaration collection
    for (const auto& v : variables_) {
        ensureVarAllocated(out, v);
    }
    // Ensure error reporting temporaries always exist; these may be referenced
    // by bounds/range checks even if the program never explicitly declares them.
    ensureVarAllocated(out, "ERR");
    ensureVarAllocated(out, "ERL");
    // 2) Arrays: allocate backing storage for all arrays discovered by semantics
    //    using their declared extents and current OPTION BASE.
    for (const auto& [name, dims] : arrayDims_) {
        long long total = 1;
        for (int ub : dims) {
            long long ext = static_cast<long long>(ub) - optionBase_ + 1;
            if (ext < 0) ext = 0;
            total *= ext;
        }
        if (total < 0) total = 0; // defensive (shouldn't happen)
        if (isStringArrayNameCG(name)) {
            ensureStringArrayAllocated(out, name, static_cast<int>(total));
        } else {
            ensureArrayAllocated(out, name, static_cast<int>(total));
        }
    }
    if (!lineNumbers_.empty()) { std::string br = "  br label %"; br += lineLabelName(lineNumbers_.front()); out << br << Symbols::LF; log() << "entry -> " << br << Symbols::LF; }
    else { out << "  ret i32 0" << Symbols::LF; out << "}" << Symbols::LF; }
}

} // namespace gwbasic
