// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_MIDASSIGNSTMT_H
#define BASIC_COMPILER_AST_MIDASSIGNSTMT_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: MidAssignStmt
 * Purpose:
 *  - GW-BASIC MID$ assignment statement: MID$(s$|A$(i[,j...]), start[, len]) = expr$
 * Inputs:
 *  - name: target string variable name (scalar)
 *  - start: numeric expression (1-based)
 *  - len: optional numeric expression length
 *  - value: string expression to copy from
 */
struct MidAssignStmt : ASTLeaf<NodeKind::MidAssignStmt, Stmt> {
    std::string name;
    std::vector<std::unique_ptr<Expr>> indices; // optional: when targeting array element (A$(...))
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> len; // optional
    std::unique_ptr<Expr> value;
    MidAssignStmt() = default;
    MidAssignStmt(std::string target_name,
                  std::vector<std::unique_ptr<Expr>> index_exprs,
                  std::unique_ptr<Expr> start_expr,
                  std::unique_ptr<Expr> len_expr,
                  std::unique_ptr<Expr> value_expr)
        : name(std::move(target_name)),
          indices(std::move(index_exprs)),
          start(std::move(start_expr)),
          len(std::move(len_expr)),
          value(std::move(value_expr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_MIDASSIGNSTMT_H
