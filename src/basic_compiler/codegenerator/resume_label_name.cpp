// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

std::string CodeGenerator::resumeLabelName(int ln, int stmtIndex) {
    std::string s = "resume_l";
    s += std::to_string(ln);
    s += "_";
    s += std::to_string(stmtIndex);
    return s;
}

} // namespace gwbasic

