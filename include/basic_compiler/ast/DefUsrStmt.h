// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DEFUSRSTMT_H
#define BASIC_COMPILER_AST_DEFUSRSTMT_H

#include <memory>
#include <optional>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

// DEF USR[digits] = expr (address)
struct DefUsrStmt final : ASTLeaf<NodeKind::DefUsrStmt, Stmt> {
    std::optional<int> index; // optional USR index (e.g., USR0...USR9); not used semantically here
    std::unique_ptr<Expr> address;
    DefUsrStmt(std::optional<int> index_opt, std::unique_ptr<Expr> address_expr)
        : index(index_opt), address(std::move(address_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DEFUSRSTMT_H
