// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

void SemanticAnalyzer::analyzeLine(const Line& line) {
    currentLine_ = line.number;
    std::ostringstream m; m << "Line " << currentLine_; log(m.str());
    for (const auto& st : line.statements) analyzeStmt(st.get());
}

} // namespace gwbasic

