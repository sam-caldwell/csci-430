// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_LINEINPUTSTMT_H
#define BASIC_COMPILER_AST_LINEINPUTSTMT_H

#include <string>
#include <utility>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

    LineInputStmt(const int channel_num, std::string var_name)
        : channel(channel_num), name(std::move(var_name)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_LINEINPUTSTMT_H
