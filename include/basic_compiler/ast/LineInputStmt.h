// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: LineInputStmt
 * Purpose:
 *  - LINE INPUT [#n,] var$ — read an entire line.
 * Inputs:
 *  - channel: Optional channel (-1 means stdin)
 *  - name: String variable name to store the line
 * Outputs:
 *  - Concrete Stmt node
 */
struct LineInputStmt : ASTLeaf<NodeKind::LineInputStmt, Stmt> {
    int channel{-1}; // -1 = stdin
    std::string name; // must be string variable
    LineInputStmt() = default;

    LineInputStmt(const int ch, std::string n)
        : ASTLeaf(), channel(ch), name(std::move(n)) {}
};

} // namespace gwbasic
