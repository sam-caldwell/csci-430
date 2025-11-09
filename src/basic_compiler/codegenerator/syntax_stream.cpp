// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: syntax
 * Summary: Access the syntax log stream used for debug tracing.
 * Parameters:
 *  - (none)
 * Returns:
 *  - std::ostream&: Stream for syntax logging.
 */
std::ostream& CodeGenerator::syntax() { return syntaxLogger_.stream(); }

} // namespace gwbasic
