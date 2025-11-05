// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <optional>
#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: MidAssignStmt
 * Purpose:
 *  - GW-BASIC MID$ assignment statement: MID$(s$, start[, len]) = expr$
 * Inputs:
 *  - name: target string variable name (scalar)
 *  - start: numeric expression (1-based)
 *  - len: optional numeric expression length
 *  - value: string expression to copy from
 */
struct MidAssignStmt : ASTLeaf<NodeKind::MidAssignStmt, Stmt> {
    std::string name;
    std::unique_ptr<Expr> index; // optional: when targeting array element
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> len; // optional
    std::unique_ptr<Expr> value;
    MidAssignStmt() = default;
    MidAssignStmt(std::string n,
                  std::unique_ptr<Expr> idx,
                  std::unique_ptr<Expr> s,
                  std::unique_ptr<Expr> l,
                  std::unique_ptr<Expr> v)
        : ASTLeaf(), name(std::move(n)), index(std::move(idx)), start(std::move(s)), len(std::move(l)), value(std::move(v)) {}
};

} // namespace gwbasic
