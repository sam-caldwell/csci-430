// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_WRITESTMT_H
#define BASIC_COMPILER_AST_WRITESTMT_H

#include <memory>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {
    /**
     * Type: WriteStmt
     * Purpose:
     *  - WRITE [#n,] expr[, expr...] — print CSV-like values to a channel.
     * Inputs:
     *  - channel: Optional channel (-1 means stdout)
     *  - items: Expressions to write
     * Outputs:
     *  - Concrete Stmt node; codegen emits formatted writes per item
     */
    struct WriteStmt : ASTLeaf<NodeKind::WriteStmt, Stmt> {
        int channel{-1};
        std::vector<std::unique_ptr<Expr> > items;

        WriteStmt() = default;

        explicit WriteStmt(const int ch, std::vector<std::unique_ptr<Expr> > xs)
            : ASTLeaf(), channel(ch), items(std::move(xs)) {
        }
    };

    // FileInputStmt and LineInputStmt moved to their own headers.
} // namespace gwbasic

#endif // BASIC_COMPILER_AST_WRITESTMT_H
