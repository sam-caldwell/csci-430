// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include <cctype>

namespace gwbasic {

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

