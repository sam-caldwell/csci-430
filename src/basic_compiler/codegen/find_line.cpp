// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: findLine
 * Summary: Lookup a Line AST node by number.
 * Parameters:
 *  - ln: Line number to look up.
 * Returns:
 *  - const Line*: Pointer to the Line AST node or nullptr.
 */
const Line* CodeGenerator::findLine(int ln) const {
    const auto it = lineMap_.find(ln);
    if (it == lineMap_.end()) return nullptr;
    return it->second;
}

} // namespace gwbasic
