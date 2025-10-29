// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <vector>
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: Line
 * Purpose:
 *  - Represent a numbered GW-BASIC line containing zero or more statements.
 * Inputs:
 *  - number: Line number (1..N, increasing)
 *  - statements: Owned list of statements in execution order
 * Outputs:
 *  - Structural node used to organize Program content
 * Theory of operation:
 *  - Parser groups parsed statements under their originating line number.
 */
struct Line {
    int number{0};
    std::vector<std::unique_ptr<Stmt>> statements;
};

} // namespace gwbasic
