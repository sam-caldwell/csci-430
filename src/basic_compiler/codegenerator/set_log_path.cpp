// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

void CodeGenerator::setLogPath(const std::string& path) {
    codegenLogger_.open(path, /*append=*/false);
    codegenLogger_.setEnabled(true);
}

} // namespace gwbasic

