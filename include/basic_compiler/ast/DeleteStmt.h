// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <optional>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * DeleteStmt: DELETE [start] [- [end]]
 *
 * Purpose:
 *  - Delete program line ranges from memory. In the compiled binary this
 *    is treated as a compile-time directive: the specified program lines
 *    are eliminated from the emitted IR. No code is generated for deleted
 *    lines and they are omitted from LIST/LLIST.
 * Inputs:
 *  - startLine/endLine: optional inclusive bounds; when omitted, covers all lines.
 *  - startIsDot/endIsDot: true if the bound was '.' (the current line).
 */
struct DeleteStmt final : ASTLeaf<NodeKind::DeleteStmt, Stmt> {
    std::optional<int> startLine;
    std::optional<int> endLine;
    bool startIsDot{false};
    bool endIsDot{false};
};

} // namespace gwbasic
