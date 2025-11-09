// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

std::string CodeGenerator::globalStringName(int id) {
    std::string s = "@.str.";
    s += std::to_string(id);
    return s;
}

} // namespace gwbasic

