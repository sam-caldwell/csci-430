// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: EndStmt
 * Purpose:
 *  - Terminate program execution.
 * Inputs: none
 * Outputs:
 *  - Concrete Stmt node; codegen emits a return from main
 * Theory of operation:
 *  - Subsequent lines are not executed.
 */
struct EndStmt : Stmt {};

} // namespace gwbasic
