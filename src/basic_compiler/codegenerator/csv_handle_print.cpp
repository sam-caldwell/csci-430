// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::csvHandlePrint
 * Purpose: Collect variables and string literals referenced by PRINT.
 */
void CodeGenerator::csvHandlePrint(const PrintStmt* p) {
    auto visitExpr = [&](const Expr* v) {
        collectExprVars(v);
        if (const auto se = dyn_cast<StringExpr>(v)) {
            if (!strLiteralId_.contains(se->value))
                strLiteralId_[se->value] = strCounter_++;
            logSem() << "StringLiteral @ " << se->pos.line << ':' << se->pos.col << Symbols::LF;
        }
    };
    if (p->value)
        visitExpr(p->value.get());
    for (const auto& vx : p->more)
        visitExpr(vx.get());
}

} // namespace gwbasic

