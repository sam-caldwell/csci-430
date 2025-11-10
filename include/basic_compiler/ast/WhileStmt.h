// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_WHILESTMT_H
#define BASIC_COMPILER_AST_WHILESTMT_H

#include <memory>
#include <utility>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: WhileStmt
 * Purpose: Single-line WHILE ... WEND loop with inline body.
 * Inputs:
 *  - cond: comparison expression
 *  - body: statements between WHILE and WEND on the same line
 */
struct WhileStmt : ASTLeaf<NodeKind::WhileStmt, Stmt> {
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Stmt>> body;
    bool inlineWend{false};
    explicit WhileStmt(std::unique_ptr<Expr> cond_expr)
        : cond(std::move(cond_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_WHILESTMT_H
