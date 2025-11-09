// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

std::string CodeGenerator::nextTemp() {
    std::string s = "%t";
    s += std::to_string(++tempCounter_);
    return s;
}

} // namespace gwbasic

