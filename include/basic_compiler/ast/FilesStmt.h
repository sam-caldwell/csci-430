// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/** FilesStmt: FILES [device$,] [path_or_pattern$] */
struct FilesStmt final : ASTLeaf<NodeKind::FilesStmt, Stmt> {
    std::unique_ptr<Expr> device;   // optional: e.g., "SCRN:", "LPT1:"
    std::unique_ptr<Expr> pattern;  // optional: directory or glob pattern
    FilesStmt() = default;
    FilesStmt(std::unique_ptr<Expr> dev, std::unique_ptr<Expr> pat)
        : ASTLeaf(), device(std::move(dev)), pattern(std::move(pat)) {}
};

} // namespace gwbasic

