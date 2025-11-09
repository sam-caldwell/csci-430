// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <map>
#include <sstream>

namespace gwbasic {

/*
 * Function: generate
 * Summary: Generate LLVM IR for a Program AST.
 * Parameters:
 *  - program: Program AST to compile to IR.
 * Returns:
 *  - std::string: Complete LLVM IR text for the program.
 * Theory of operation:
 *  - Collect declarations, emit header/globals and a function prologue.
 *    Iterate source lines in ascending order emitting basic blocks and
 *    control flow, then emit the function epilogue and return the IR.
 */
std::string CodeGenerator::generate(const Program& program) {
    collectDecls(program);
    std::ostringstream out;
    emitHeader(out);
    emitGlobals(out);
    emitMainPrologue(out);
    if (!lineNumbers_.empty()) {

        const int lastIdx = static_cast<int>(lineNumbers_.size() - 1);
        std::map<int, const Line*> lm;
        for (const auto& l : program.lines) lm[l.number] = &l;
        for (int i = 0; i <= lastIdx; ++i) {
            int ln = lineNumbers_[i];
            auto it = lm.find(ln);
            if (it == lm.end()) throw CodeGenError("Internal: missing line AST");
            emitLineBlock(out, *it->second, i, lastIdx);
        }
        emitMainEpilogue(out);
    }
    return out.str();
}

} // namespace gwbasic
