// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_MIDASSIGNSTMT_H
#define BASIC_COMPILER_AST_MIDASSIGNSTMT_H

#include <memory>
#include <optional>
#include <string>
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
    MidAssignStmt(std::string n,
                  std::vector<std::unique_ptr<Expr>> idx,
                  std::unique_ptr<Expr> s,
                  std::unique_ptr<Expr> l,
                  std::unique_ptr<Expr> v)
        : ASTLeaf(), name(std::move(n)), indices(std::move(idx)), start(std::move(s)), len(std::move(l)), value(std::move(v)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_MIDASSIGNSTMT_H
