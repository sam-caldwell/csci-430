// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <memory>
#include <vector>
#include "basic_compiler/ast/Program.h"

namespace gwbasic {

// AstOptimizer: Performs lightweight, semantics-preserving simplifications
// on the AST prior to IR generation. Focus areas:
//  - Constant folding (arithmetic and comparisons)
//  - Unary plus elimination; unary minus folding for constants
//  - Algebraic identities (x+0, 0+x, x-0, x*1, 1*x, x/1, x*0 -> 0)
//  - IF with constant condition -> replace with GOTO or remove
//  - STEP 1.0 in FOR -> null step (uses default path in codegen)
class AstOptimizer {
public:
    void optimize(Program& program);

private:
    // Statement and expression rewriting
    std::unique_ptr<Expr> optExpr(std::unique_ptr<Expr> e);
    bool isZero(const Expr* e) const;
    bool isOne(const Expr* e) const;
    bool asNumber(const Expr* e, double& out) const;
};

} // namespace gwbasic

