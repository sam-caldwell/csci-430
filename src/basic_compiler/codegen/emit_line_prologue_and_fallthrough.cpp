// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitLinePrologue(std::ostringstream &out, const Line &line) {
    currentLine_ = line.number;
    out << lineLabelName(line.number) << ":" << Symbols::LF;
    
}

std::string CodeGenerator::nextLineLabelForIndex(const int lineIndex, const int lastIndex) const {
    if (lineIndex < lastIndex) {
        return lineLabelName(lineNumbers_[lineIndex + 1]);
    }
    return std::string("exit");
}

void CodeGenerator::emitLineFallthrough(std::ostringstream &out, const std::string &nextLabel) {
    const std::string irInstr = std::format("  br label %{}", nextLabel);
    out << irInstr << Symbols::LF;
    
}

} // namespace gwbasic
