// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Line.h"

namespace gwbasic {

/*
 * Function: findLine
 * Summary: Lookup a Line AST node by number.
 * Parameters:
 *  - ln: Line number to look up.
 * Returns:
 *  - const Line*: Pointer to the Line AST node or nullptr.
 */
const Line* CodeGenerator::findLine(int line) const {
    const auto iter = lineMap_.find(line);
    if (iter == lineMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

} // namespace gwbasic
