// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

/*
 * Function: setSemanticLogPath
 * Summary: Configure and enable the semantic analysis log.
 * Parameters:
 *  - path: Destination file path for semantic logs.
 * Returns:
 *  - void
 */
void CodeGenerator::setSemanticLogPath(const std::string& path) {
    semLogger_.open(path, /*append=*/false);
    semLogger_.setEnabled(true);
}

} // namespace gwbasic
