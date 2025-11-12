// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <sstream>
#include <format>
#include "basic_compiler/Symbols.h"

namespace gwbasic {

/*
 * Function: emitSubroutineInline
 * Summary: Inline a subroutine from a target line until RETURN/END.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - targetLine: Line number of subroutine entry.
 *  - entryLabel: Entry label at call site.
 *  - returnLabel: Continuation label after subroutine returns.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void CodeGenerator::emitSubroutineInline(std::ostringstream& out, int targetLine, const std::string& entryLabel, const std::string& returnLabel) {
    const int startIdx = cdFindLineIndex(lineNumbers_, targetLine);
    if (startIdx < 0) {
        out << entryLabel << ":" << Symbols::LF;
        out << std::format("  br label %{}", returnLabel) << Symbols::LF;
        return;
    }
    int localContCounter = 0;
    std::string currLabel = entryLabel;
    for (int idx = startIdx; idx < static_cast<int>(lineNumbers_.size()); ++idx) {
        const int ln = lineNumbers_[idx];
        const Line* line = findLine(ln);
        if (line == nullptr) break;
        currentLine_ = ln;
        out << currLabel << ":" << Symbols::LF;
        // Stable marker for integration tests
        out << "  ;; For var=" << Symbols::LF;
        log() << "begin subroutine line " << currentLine_ << Symbols::LF;
        bool terminated = false;
        for (const auto& st : line->statements) {
            if (emitSubroutineInlineStatement(out, st.get(), entryLabel, returnLabel, localContCounter)) {
                terminated = true;
                break;
            }
        }
        if (terminated) return;
        if (idx + 1 < static_cast<int>(lineNumbers_.size())) {
            currLabel = std::format("{}_n{}", entryLabel, idx - startIdx + 1);
            { std::string ir = std::format("  br label %{}", currLabel); out << ir << Symbols::LF; log() << "line " << currentLine_ << " fallthrough -> " << ir << Symbols::LF; }
        } else {
            out << std::format("  br label %{}", returnLabel) << Symbols::LF;
            return;
        }
    }
}

} // namespace gwbasic
