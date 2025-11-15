// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: ast_optimizer_stmt.cpp
 * Purpose:
 *  - Implement statement-level optimizations for `AstOptimizer`, including
 *    IF-constant reduction and FOR loop canonicalization.
 * Theory of operation:
 *  - Walks the program lines and rewrites statements in-place, delegating
 *    expression simplification to `optExpr`.
 */
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/Program.h"

namespace gwbasic {

/**
 * Function: AstOptimizer::optimize
 * Purpose:
 *  - Apply a series of semantics-preserving rewrites to the program AST
 *    to simplify downstream code generation.
 * Inputs:
 *  - program: Mutable AST root to optimize
 * Effects:
 *  - Mutates expressions and statements in-place; removes or replaces
 *    statements when provably redundant.
 * Details:
 *  - Expression trees are simplified by `optExpr`.
 *  - IF with constant condition: replace it with `GOTO` if true; drop if false.
 *  - FOR: simplify start/end/step; elide step if it becomes 1.0.
 */
void AstOptimizer::optimize(Program& program) {
    for (auto& [number, statements] : program.lines) {
        optimizeLineStatements(statements);
    }
}

} // namespace gwbasic
