// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

/*
 * Function: csvHandlePrint
 * Summary: Collect variables and string literals referenced by PRINT.
 * Parameters:
 *  - print_stmt: Parsed PrintStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandlePrint(const PrintStmt* print_stmt) { // NOLINT(readability-function-size)
    auto visitExpr = [&](const Expr* expr) {
        collectExprVars(expr);
        if (const auto* const stringExpr = dyn_cast<StringExpr>(expr)) {
            if (!strLiteralId_.contains(stringExpr->value)) {
                strLiteralId_[stringExpr->value] = strCounter_++;
            }
            logSem() << "StringLiteral @ " << stringExpr->pos.line << ':'
                     << stringExpr->pos.col << Symbols::LF;
        }
    };
    if (print_stmt->value) {
        visitExpr(print_stmt->value.get());
    }
    for (const auto& moreExpr : print_stmt->more) {
        visitExpr(moreExpr.get());
    }
}

} // namespace gwbasic
