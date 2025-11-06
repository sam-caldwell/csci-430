// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>
#include <unordered_set>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::analyze
 * Inputs:
 *  - program: Parsed Program AST
 * Outputs:
 *  - SemanticAnalyzer::Result: Collected vars/strings/lines/common names
 * Theory of operation:
 *  - Resets state, validates unique line numbers, analyzes each line's
 *    statements, and returns aggregated semantic information.
 */
SemanticAnalyzer::Result SemanticAnalyzer::analyze(const Program& program) {
    scopes_.clear(); scopes_.emplace_back();
    vars_.clear(); strings_.clear(); lines_.clear();
    currentLine_ = 0;
    std::unordered_set<int> seen;
    for (const auto& line : program.lines) {
        if (!seen.insert(line.number).second) {
            std::ostringstream err; err << "ControlFlowError: duplicate line number " << line.number; log() << err.str() << '\n';
            throw SemanticError(err.str());
        }
        lines_.insert(line.number);
    }
    for (const auto& line : program.lines) analyzeLine(line);
    Result r; r.variables = vars_; r.stringLiterals = strings_; r.lineNumbers = lines_; r.commonVariables = common_; r.arrays = arrays_; r.userFunctions = userFunctions_; r.optionBase = optionBase_;
    // Determine which variables are strings by suffix or DEFSTR mapping
    for (const auto& v : vars_) {
        if (varNameIsString(v)) r.stringVariables.insert(v);
    }
    // Determine numeric kinds for non-string variables
    for (const auto& v : vars_) {
        if (!r.stringVariables.contains(v)) {
            r.numericKinds[v] = numericKindOf(v);
        }
    }
    return r;
}

} // namespace gwbasic
