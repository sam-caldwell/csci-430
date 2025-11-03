// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Expr.h"

namespace gwbasic {

/**
 * Type: AstOptimizer
 * Purpose:
 *  - Perform lightweight, semantics-preserving simplifications on the AST
 *    prior to IR generation.
 * Focus areas:
 *  - Constant folding (arithmetic and comparisons)
 *  - Unary plus elimination; unary minus folding for constants
 *  - Algebraic identities (x+0, 0+x, x-0, x*1, 1*x, x/1, x*0 -> 0)
 *  - IF with constant condition -> replace with GOTO or remove
 *  - STEP 1.0 in FOR -> null step (use a default path in codegen)
 * Theory of operation:
 *  - The optimizer walks statements and expressions, rewriting in place.
 *    Expression-level rules are defined in a separate translation unit
 *    from statement-level rules for clarity and maintainability.
 */
class AstOptimizer {
public:
    /**
     * Function: AstOptimizer::optimize
     * Purpose:
     *  - Apply statement- and expression-level simplifications to a program.
     * Inputs:
     *  - program: AST root to mutate in place
     * Effects:
     *  - Rewrites expressions and statements; may remove or replace
     *    statements when provably redundant.
     */
    static auto optimize(Program &program) -> void;

private:
    /**
     * Function: AstOptimizer::optExpr
     * Purpose:
     *  - Simplify an expression tree via constant folding and algebraic
     *    identities. Eliminates unary plus and folds unary minus for
     *    numeric literals.
     * Inputs:
     *  - e: Owned expression to simplify (may be null)
     * Outputs:
     *  - Returns simplified expression, possibly a new node.
     */
    static std::unique_ptr<Expr> optExpr(std::unique_ptr<Expr> e);

    /**
     * Function: AstOptimizer::isZero
     * Purpose:
     *  - Determine whether an expression is the numeric constant 0.0.
     * Inputs:
     *  - e: Expression node to test (may be null)
     * Outputs:
     *  - bool: true if 'e' is a NumberExpr with value 0.0
     */
    static bool isZero(const Expr* e);

    /**
     * Function: AstOptimizer::isOne
     * Purpose:
     *  - Determine whether an expression is the numeric constant 1.0.
     * Inputs:
     *  - e: Expression node to test (may be null)
     * Outputs:
     *  - bool: true if 'e' is a NumberExpr with value 1.0
     */
    static bool isOne(const Expr* e) ;

    /**
     * Function: AstOptimizer::asNumber
     * Purpose:
     *  - Extract a numeric value when the expression is a NumberExpr.
     * Inputs:
     *  - e: Expression node to inspect
     *  - out: Reference to receive the numeric value
     * Outputs:
     *  - bool: true if 'e' is NumberExpr and 'out' was set; false otherwise
     */
    static bool asNumber(const Expr* e, double& out);
};

} // namespace gwbasic
