// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: log
 * Summary: Access the code generation log stream.
 * Parameters:
 *  - (none)
 * Returns:
 *  - std::ostream&: Stream for codegen logging.
 */
std::ostream& CodeGenerator::log() {
    return codegenLogger_.stream();
}

} // namespace gwbasic
