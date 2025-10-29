// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

const Line* CodeGenerator::findLine(int ln) const {
    /*
     * Function: CodeGenerator::findLine
     * Inputs:
     *  - ln: line number to look up
     * Outputs:
     *  - const Line*: pointer to the Line AST node or nullptr
     * Theory of operation:
     *  - Performs a map lookup using the prebuilt lineMap_.
     */
    auto it = lineMap_.find(ln);
    if (it == lineMap_.end()) return nullptr;
    return it->second;
}

} // namespace gwbasic
