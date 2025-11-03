// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {
    /**
     * Type: WriteStmt
     * Purpose:
     *  - WRITE [#n,] expr[, expr...]
     */
    struct WriteStmt : ASTLeaf<NodeKind::WriteStmt, Stmt> {
        int channel{-1};
        std::vector<std::unique_ptr<Expr> > items;

        WriteStmt() = default;

        explicit WriteStmt(const int ch, std::vector<std::unique_ptr<Expr> > xs)
            : ASTLeaf(), channel(ch), items(std::move(xs)) {
        }
    };

    /**
     * Type: FileInputStmt
     * Purpose:
     *  - INPUT #n, var[, var...]
     */
    struct FileInputStmt : ASTLeaf<NodeKind::FileInputStmt, Stmt> {
        int channel{1};
        std::vector<std::string> variables; // names only; array targets not yet supported here
        FileInputStmt() = default;

        explicit FileInputStmt(const int ch, std::vector<std::string> vars) : ASTLeaf(), channel(ch),
                                                                        variables(std::move(vars)) {
        }
    };

    /**
     * Type: LineInputStmt
     * Purpose:
     *  - LINE INPUT [#n,] var$ (reads a whole line)
     */
    struct LineInputStmt : ASTLeaf<NodeKind::LineInputStmt, Stmt> {
        int channel{-1}; // -1 = stdin
        std::string name; // must be string variable
        LineInputStmt() = default;

        LineInputStmt(const int ch, std::string n) : ASTLeaf(), channel(ch), name(std::move(n)) {
        }
    };
} // namespace gwbasic
