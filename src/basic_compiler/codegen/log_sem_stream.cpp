// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <ostream>

namespace gwbasic {

/*
 * Function: logSem
 * Summary: Access the semantic analysis log stream.
 * Parameters:
 *  - (none)
 * Returns:
 *  - std::ostream&: Stream for semantic logging.
 */
std::ostream& CodeGenerator::logSem() {
    return semLogger_.stream();
}

} // namespace gwbasic
