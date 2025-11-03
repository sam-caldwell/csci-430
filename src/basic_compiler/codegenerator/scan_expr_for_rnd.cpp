// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <cctype>

namespace gwbasic {

/*
 * Function: CodeGenerator::scanExprForRnd
 * Inputs:
 *  - e: Expression node to scan
 * Outputs:
 *  - void (sets internal flag when RND is referenced)
 * Theory of operation:
 *  - Recursively traverses the expression; on CallExpr, normalizes the
 *    callee name and marks needsRndHelper_ if it is 'RND'.
 */
void CodeGenerator::scanExprForRnd(const Expr* e) {
    if (!e) return;
    if (const auto c = dyn_cast<const CallExpr>(e)) {
        std::string fn = c->callee; for (auto& ch : fn) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (fn == "RND") needsRndHelper_ = true;
        for (const auto& a : c->args) scanExprForRnd(a.get());
        return;
    }
    if (const auto b = dyn_cast<const BinaryExpr>(e)) { scanExprForRnd(b->lhs.get()); scanExprForRnd(b->rhs.get()); return; }
    if (const auto u = dyn_cast<const UnaryExpr>(e)) { scanExprForRnd(u->inner.get()); return; }
}

} // namespace gwbasic
