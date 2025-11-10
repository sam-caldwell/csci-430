// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_LISTSTMT_H
#define BASIC_COMPILER_AST_LISTSTMT_H

#include <optional>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * ListStmt: LIST/LLIST [start] [- [end]]
 *
 * Purpose:
 *  - Emit a listing of program lines to screen (LIST) or printer (LLIST).
 * Inputs:
 *  - startLine/endLine: optional inclusive bounds; when omitted, covers all lines.
 *  - startIsDot/endIsDot: true if the bound was '.' (current line at compile site).
 *  - toPrinter: true for LLIST (printer channel), false for LIST (stdout).
 */
struct ListStmt final : ASTLeaf<NodeKind::ListStmt, Stmt> {
    std::optional<int> startLine;
    std::optional<int> endLine;
    bool startIsDot{false};
    bool endIsDot{false};
    bool toPrinter{false};
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_LISTSTMT_H
