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
#include "basic_compiler/ast/RTTI.h"

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
    for (auto&[number, statements] : program.lines) {
        std::vector<std::unique_ptr<Stmt>> newStmts;
        newStmts.reserve(statements.size());
        for (auto& st : statements) {
            if (const auto asg = dyn_cast<AssignStmt>(st.get())) {
                asg->value = optExpr(std::move(asg->value));
                newStmts.emplace_back(std::move(st));
            } else if (const auto pr = dyn_cast<PrintStmt>(st.get())) {
                pr->value = optExpr(std::move(pr->value));
                newStmts.emplace_back(std::move(st));
            } else if (const auto is = dyn_cast<IfStmt>(st.get())) {
                is->cond = optExpr(std::move(is->cond));
                if (double v; asNumber(is->cond.get(), v)) {
                    if (v != 0.0) {
                        // Replace with GOTO target
                        auto g = std::make_unique<GotoStmt>(is->targetLine);
                        g->pos = is->pos;
                        newStmts.emplace_back(std::move(g));
                    } else {
                        // Remove statement (no-op)
                    }
                } else {
                    newStmts.emplace_back(std::move(st));
                }
            } else if (const auto fs = dyn_cast<ForStmt>(st.get())) {
                fs->start = optExpr(std::move(fs->start));
                fs->end   = optExpr(std::move(fs->end));
                if (fs->step) fs->step = optExpr(std::move(fs->step));
                // If step simplifies to 1.0, drop it to trigger default path in codegen
                if (fs->step && isOne(fs->step.get())) fs->step.reset();
                // Optimize body
                std::vector<std::unique_ptr<Stmt>> body;
                body.reserve(fs->body.size());
                for (auto& bs : fs->body) {
                    if (const auto basg = dyn_cast<AssignStmt>(bs.get())) {
                        basg->value = optExpr(std::move(basg->value));
                        body.emplace_back(std::move(bs));
                    } else if (const auto bpr = dyn_cast<PrintStmt>(bs.get())) {
                        bpr->value = optExpr(std::move(bpr->value));
                        body.emplace_back(std::move(bs));
                    } else {
                        // leave as-is; other constructs in FOR body unchanged
                        body.emplace_back(std::move(bs));
                    }
                }
                fs->body = std::move(body);
                newStmts.emplace_back(std::move(st));
            } else {
                // Other statements: GOTO/GOSUB/RETURN/END/INPUT left unchanged
                newStmts.emplace_back(std::move(st));
            }
        }
        statements = std::move(newStmts);
    }
}

} // namespace gwbasic
