// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <vector>
#include "basic_compiler/ast/Line.h"


namespace gwbasic {

/**
 * Type: Program
 * Purpose:
 *  - Top-level container for an entire GW-BASIC program, preserving line
 *    order and associated statements.
 * Inputs:
 *  - lines: Sequence of Line nodes, ascending by line number
 * Outputs:
 *  - Structural root consumed by the CodeGenerator
 * Theory of operation:
 *  - Parser builds and returns a Program; downstream phases traverse it to
 *    analyze and emit LLVM IR.
 */
struct Program {
    std::vector<Line> lines;
};

} // namespace gwbasic
