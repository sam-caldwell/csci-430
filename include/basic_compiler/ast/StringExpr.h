// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_STRINGEXPR_H
#define BASIC_COMPILER_AST_STRINGEXPR_H

#include <string>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: StringExpr
 * Purpose:
 *  - Represent a string literal in the AST (without surrounding quotes).
 * Inputs:
 *  - value: Raw string contents
 * Outputs:
 *  - Concrete Expr node used by codegen to place literal data in .rodata
 * Theory of operation:
 *  - Codegen interns literals and emits global string constants with
 *    references via getelementptr for @printf calls.
 */
struct StringExpr : ASTLeaf<NodeKind::StringExpr, Expr> {
    std::string value;
    explicit StringExpr(std::string v) : ASTLeaf(), value(std::move(v)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_STRINGEXPR_H
