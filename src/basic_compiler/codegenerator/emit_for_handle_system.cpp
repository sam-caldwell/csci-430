// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: emitForHandleSystem
 * Summary: Emit SYSTEM handling inside a FOR body (exit program).
 * Parameters:
 *  - out: IR output stream to append to.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandleSystem(std::ostringstream& out) {
    out << "  br label %exit" << Symbols::LF;
}

} // namespace gwbasic
