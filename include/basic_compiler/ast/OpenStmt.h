// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

enum class FileMode { Input, Output };

/**
 * Type: OpenStmt
 * Purpose:
 *  - OPEN <string> FOR (INPUT|OUTPUT) AS #<n> — open a file channel.
 * Inputs:
 *  - filename: String expression naming the file
 *  - mode: FileMode (Input or Output)
 *  - channel: Logical channel number (1..16)
 * Outputs:
 *  - Concrete Stmt node; semantics validate range and types
 */
struct OpenStmt final : ASTLeaf<NodeKind::OpenStmt, Stmt> {
    std::unique_ptr<Expr> filename;
    FileMode mode{FileMode::Input};
    int channel{1};
    OpenStmt() = default;
    OpenStmt(std::unique_ptr<Expr> f, const FileMode m, const int ch)
        : ASTLeaf(), filename(std::move(f)), mode(m), channel(ch) {}
};

} // namespace gwbasic
