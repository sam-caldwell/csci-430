// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::emitForHandleSystem
 * Purpose: Emit SYSTEM handling inside a FOR body (exits program).
 */
void CodeGenerator::emitForHandleSystem(std::ostringstream& out) {
    out << "  br label %exit" << Symbols::LF;
}

} // namespace gwbasic

