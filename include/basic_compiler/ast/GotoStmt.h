// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: GotoStmt
 * Purpose:
 *  - Unconditional branch to a specific line number.
 * Inputs:
 *  - targetLine: Destination program line
 * Outputs:
 *  - Concrete Stmt node; codegen emits an unconditional br to label
 * Theory of operation:
 *  - Line numbers are mapped to basic blocks with labels “line_<N>”.
 */
struct GotoStmt : Stmt {
    int targetLine;
    explicit GotoStmt(int ln) : targetLine(ln) {}
};

} // namespace gwbasic
