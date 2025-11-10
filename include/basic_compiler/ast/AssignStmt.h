// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ASSIGNSTMT_H
#define BASIC_COMPILER_AST_ASSIGNSTMT_H

#include <memory>
#include <string>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: AssignStmt
 * Purpose:
 *  - LET or implicit assignment of an expression to a variable.
 * Inputs:
 *  - name: Variable identifier
 *  - value: Expression to evaluate and store
 * Outputs:
 *  - Concrete Stmt node; codegen ensures allocation and store to the symbol
 * Theory of operation:
 *  - Codegen emits store to an alloca location tracked per variable name.
 */
struct AssignStmt : ASTLeaf<NodeKind::AssignStmt, Stmt> {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string nameStr, std::unique_ptr<Expr> valueExpr)
        : name(std::move(nameStr)), value(std::move(valueExpr)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ASSIGNSTMT_H
