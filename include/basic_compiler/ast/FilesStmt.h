// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_FILESSTMT_H
#define BASIC_COMPILER_AST_FILESSTMT_H

#include <memory>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_FILESSTMT_H
