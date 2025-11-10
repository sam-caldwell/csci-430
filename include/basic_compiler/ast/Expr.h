// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_EXPR_H
#define BASIC_COMPILER_AST_EXPR_H

#include "basic_compiler/ast/Node.h"
#include "basic_compiler/ast/NodeKind.h"

namespace gwbasic {

/**
 * Type: Expr (abstract)
 * Purpose:
 *  - Common base for all expression nodes (numeric, string, variable,
 *    unary/binary operations, etc.).
 * Inputs:
 *  - pos: Source position captured by the parser for diagnostics/logging
 * Outputs:
 *  - Virtual base enabling polymorphic handling of expressions
 * Theory of operation:
 *  - Concrete subclasses implement specific expression forms. Ownership is
 *    typically via std::unique_ptr<Expr> in parent nodes.
 */
struct Expr : Node {
    // ReSharper disable once CppEnforceOverridingDestructorStyle
    ~Expr() override = default;
protected:
    explicit Expr(const NodeKind k) : Node(k) {}
    Expr() : Node(NodeKind::AbstractExpr) {}
public:
    static bool classof(const Node* N) {
        if (!N) return false;
        switch (N->kind) {
            case NodeKind::AbstractExpr:
            case NodeKind::NumberExpr:
            case NodeKind::StringExpr:
            case NodeKind::VarExpr:
            case NodeKind::UnaryExpr:
            case NodeKind::BinaryExpr:
                return true;
            default:
                return false;
        }
    }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_EXPR_H
