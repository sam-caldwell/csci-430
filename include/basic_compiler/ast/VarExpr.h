// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_VAREXPR_H
#define BASIC_COMPILER_AST_VAREXPR_H

#include <string>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: VarExpr
 * Purpose:
 *  - Reference to a scalar variable by name.
 * Inputs:
 *  - name: Identifier (case-insensitive in BASIC semantics; stored raw)
 * Outputs:
 *  - Concrete Expr node; codegen ensures allocation and loads/stores as needed
 * Theory of operation:
 *  - Codegen maintains a symbol table mapping variable names to allocas
 *    within the current function scope.
 */
struct VarExpr : ASTLeaf<NodeKind::VarExpr, Expr> {
    std::string name;
    explicit VarExpr(std::string name_str) : name(std::move(name_str)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_VAREXPR_H
