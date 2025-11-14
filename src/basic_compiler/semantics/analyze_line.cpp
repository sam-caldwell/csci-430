// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/Line.h"

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
    log() << "Line " << currentLine_ << '\n';
    for (const auto& stmtPtr : line.statements) {
        analyzeStmt(stmtPtr.get());
    }
}

} // namespace gwbasic
