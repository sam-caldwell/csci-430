// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/semantics/SemanticError.h"

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
    for (const auto& line : program.lines) {
        analyzeLine(line);
    }
    Result result;
    result.variables = vars_;
    result.stringLiterals = strings_;
    result.lineNumbers = lines_;
    result.commonVariables = common_;
    result.arrays = allArrays_;
    result.userFunctions = userFunctions_;
    result.optionBase = optionBase_;
    result.printZones = printZones_;
    // Determine which variables are strings by suffix or DEFSTR mapping
    for (const auto& varName : vars_) {
        if (varNameIsString(varName)) { result.stringVariables.insert(varName); }
    }
    // Determine numeric kinds for non-string variables
    for (const auto& varName : vars_) {
        if (!result.stringVariables.contains(varName)) {
            result.numericKinds[varName] = numericKindOf(varName);
        }
    }
    return result;
}

} // namespace gwbasic
