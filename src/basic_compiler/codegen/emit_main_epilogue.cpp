// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: emitMainEpilogue
 * Summary: Emit exit label and return 0 to finish main.
 * Parameters:
 *  - out: IR output stream to append to.
 * Returns:
 *  - void
 */
void CodeGenerator::emitMainEpilogue(std::ostringstream& out) {
    out << "exit:" << Symbols::LF;
    out << "  ret i32 0" << Symbols::LF;
    out << "}" << Symbols::LF;
}

} // namespace gwbasic
