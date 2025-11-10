// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_IFBLOCKSTMT_H
#define BASIC_COMPILER_AST_IFBLOCKSTMT_H

#include <memory>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: IfBlockStmt
 * Purpose:
 *  - Multi-line IF ... THEN [ ... ELSE ... ] END IF block with nested bodies.
 * Inputs:
 *  - cond: Comparison expression (non-string)
 *  - thenBody: Statements executed when condition is true
 *  - elseBody: Optional statements executed when condition is false
 * Outputs:
 *  - Concrete Stmt node representing the structured IF block
 */
struct IfBlockStmt final : ASTLeaf<NodeKind::IfBlockStmt, Stmt> {
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Stmt>> thenBody;
    std::vector<std::unique_ptr<Stmt>> elseBody; // empty if no ELSE
    // True when this IF block is fully contained on a single source line.
    // Inline IF blocks should not participate in END IF folding in the
    // parser's restructuring pass.
    bool inlineEnd{false};
    explicit IfBlockStmt(std::unique_ptr<Expr> c) : ASTLeaf(), cond(std::move(c)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_IFBLOCKSTMT_H
