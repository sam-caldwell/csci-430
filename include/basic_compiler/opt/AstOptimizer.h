// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Stmt.h"

#include <memory>
#include <vector>

namespace gwbasic {

    struct ForStmt;
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
     * Function: AstOptimizer::optimizeLineStatements
     * Purpose:
     *  - Optimize a vector of statements in-place for a single program line.
     * Details:
     *  - Delegates to expression simplifier and targeted stmt re-writers.
     */
    static auto optimizeLineStatements(std::vector<std::unique_ptr<Stmt>>& statements) -> void;

    /**
     * Function: AstOptimizer::rewriteIf
     * Purpose:
     *  - Apply constant condition folding to an IF statement. May replace
     *    with a GOTO, remove it, or leave it unchanged. Appends zero or one
     *    statements to 'out'.
     */
    static auto rewriteIf(std::unique_ptr<Stmt>& statement,
                          std::vector<std::unique_ptr<Stmt>>& out) -> void;

    /**
     * Function: AstOptimizer::optimizeForBody
     * Purpose:
     *  - Simplify expressions within a FOR body and perform STEP elision
     *    handling (the STEP value is processed by the caller).
     */
    static auto optimizeForBody(ForStmt& forStmt) -> void;

    // Per-kind statement handlers (one-function-per-file definitions)
    static auto optimizeAssignStmt(std::unique_ptr<Stmt>& statement,
                                   std::vector<std::unique_ptr<Stmt>>& out) -> bool;
    static auto optimizePrintStmt(std::unique_ptr<Stmt>& statement,
                                  std::vector<std::unique_ptr<Stmt>>& out) -> bool;
    static auto optimizeForStmt(std::unique_ptr<Stmt>& statement,
                                std::vector<std::unique_ptr<Stmt>>& out) -> bool;
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
    static std::unique_ptr<Expr> optExpr(std::unique_ptr<Expr> expr);

    /**
     * Function: AstOptimizer::isZero
     * Purpose:
     *  - Determine whether an expression is the numeric constant 0.0.
     * Inputs:
     *  - e: Expression node to test (may be null)
     * Outputs:
     *  - bool: true if 'e' is a NumberExpr with value 0.0
     */
    static bool isZero(const Expr* expr);

    /**
     * Function: AstOptimizer::isOne
     * Purpose:
     *  - Determine whether an expression is the numeric constant 1.0.
     * Inputs:
     *  - e: Expression node to test (may be null)
     * Outputs:
     *  - bool: true if 'e' is a NumberExpr with value 1.0
     */
    static bool isOne(const Expr* expr) ;

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
    static bool asNumber(const Expr* expr, double& out);
};

} // namespace gwbasic
