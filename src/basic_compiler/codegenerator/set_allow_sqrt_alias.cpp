// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: setAllowSqrtAlias
 * Summary: Toggle support for SQRT aliasing in intrinsic calls.
 * Parameters:
 *  - allow: True to enable aliasing behavior.
 * Returns:
 *  - void
 */
void CodeGenerator::setAllowSqrtAlias(const bool allow) {
    allowSqrtAlias_ = allow;
}

} // namespace gwbasic
