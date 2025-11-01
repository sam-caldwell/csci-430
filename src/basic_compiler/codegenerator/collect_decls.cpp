// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <algorithm>

namespace gwbasic {

void CodeGenerator::collectDecls(const Program& program) {
    /*
     * Function: CodeGenerator::collectDecls
     * Inputs:
     *  - program: AST to be compiled
     * Outputs:
     *  - void (initializes internal maps/sets and prepares line ordering)
     * Theory of operation:
     *  - Clears internal state, scans all lines/statements to populate the
     *    sets of variables and string literals, records and sorts line numbers
     *    and builds a line-number to Line* map for later codegen.
     */
    variables_.clear();
    varAllocaName_.clear();
    strLiteralId_.clear();
    tempCounter_ = 0;
    strCounter_ = 0;
    lineNumbers_.clear();
    lineMap_.clear();

    for (const auto& line : program.lines) {
        lineNumbers_.push_back(line.number);
        lineMap_[line.number] = &line;
        if (!semProvided_) {
            for (const auto& st : line.statements) collectStmtVars(st.get());
        }
    }
    std::ranges::sort(lineNumbers_);
    lineNumbers_.erase(std::ranges::unique(lineNumbers_).begin(), lineNumbers_.end());

    if (semProvided_) {
        // Seed variables and strings from semantics
        variables_ = semVariables_;
        strLiteralId_.clear();
        strCounter_ = 0;
        for (const auto& s : semStrings_) {
            // Only assign ids once; stable deterministic order
            if (!strLiteralId_.contains(s)) strLiteralId_[s] = strCounter_++;
        }
        // LineNumbers are computed from AST to drive emission order; no change
    }
}

} // namespace gwbasic
