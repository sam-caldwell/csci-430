// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::logLine(std::string_view message) {
    std::ostringstream m;
    m << "line " << currentLine_ << ' ' << message;
    log() << m.str() << Symbols::LF;
}

} // namespace gwbasic

