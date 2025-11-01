// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>
#include <unordered_set>

namespace gwbasic {

SemanticAnalyzer::Result SemanticAnalyzer::analyze(const Program& program) {
    scopes_.clear(); scopes_.emplace_back();
    vars_.clear(); strings_.clear(); lines_.clear();
    currentLine_ = 0;
    std::unordered_set<int> seen;
    for (const auto& line : program.lines) {
        if (!seen.insert(line.number).second) {
            std::ostringstream err; err << "ControlFlowError: duplicate line number " << line.number; log(err.str());
            throw SemanticError(err.str());
        }
        lines_.insert(line.number);
    }
    for (const auto& line : program.lines) analyzeLine(line);
    Result r; r.variables = vars_; r.stringLiterals = strings_; r.lineNumbers = lines_; return r;
}

} // namespace gwbasic

