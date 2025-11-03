// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

enum class FileMode { Input, Output };

struct OpenStmt final : ASTLeaf<NodeKind::OpenStmt, Stmt> {
    std::unique_ptr<Expr> filename;
    FileMode mode{FileMode::Input};
    int channel{1};
    OpenStmt() = default;
    OpenStmt(std::unique_ptr<Expr> f, const FileMode m, const int ch)
        : ASTLeaf(), filename(std::move(f)), mode(m), channel(ch) {}
};

struct CloseStmt final : ASTLeaf<NodeKind::CloseStmt, Stmt> {
    int channel{1};
    explicit CloseStmt(const int ch) : ASTLeaf(), channel(ch) {}
};

} // namespace gwbasic

