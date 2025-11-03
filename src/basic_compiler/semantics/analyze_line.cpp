// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::analyzeLine
 * Inputs:
 *  - line: Program line node to analyze
 * Outputs:
 *  - void (logs line context and analyzes contained statements)
 * Theory of operation:
 *  - Sets currentLine_, logs it, and analyzes each statement in order.
 */
void SemanticAnalyzer::analyzeLine(const Line& line) {
    currentLine_ = line.number;
    std::ostringstream m; m << "Line " << currentLine_; log(m.str());
    for (const auto& st : line.statements) analyzeStmt(st.get());
}

} // namespace gwbasic
