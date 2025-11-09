// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

void CodeGenerator::setSemanticLogPath(const std::string& path) {
    semLogger_.open(path, /*append=*/false);
    semLogger_.setEnabled(true);
}

} // namespace gwbasic

