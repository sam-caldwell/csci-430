// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitSubHandleIf
 * Summary: Emit single-line IF ... THEN <line> in a subroutine body.
 * Parameters:
 *  - out: IR output stream.
 *  - is: IfStmt node (comparison to line target branch).
 *  - entryLabel: Current subroutine entry label.
 *  - localCounter: Per-line counter for unique labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitSubHandleIf(std::ostringstream& out, const IfStmt* ifStmt, std::string_view entryLabel, int& localCounter) {
    auto* binExpr = dyn_cast<BinaryExpr>(ifStmt->cond.get());
    if (binExpr == nullptr || (binExpr->op != BinaryOp::Eq && binExpr->op != BinaryOp::Ne && binExpr->op != BinaryOp::Lt && binExpr->op != BinaryOp::Le && binExpr->op != BinaryOp::Gt && binExpr->op != BinaryOp::Ge)) {
        throw CodeGenError("IF condition must be a comparison");
    }
    std::string cond = emitComparison(out, binExpr);
    std::string contLbl = std::format("{}_cont{}", entryLabel, ++localCounter);
    const std::string irLine = std::format("  br i1 {}, label %{}, label %{}", cond, lineLabelName(ifStmt->targetLine), contLbl);
    out << irLine << Symbols::LF;
    // Also embed a comment line so integration tests can match the marker in IR
    out << "  ;; IfStmt -> " << irLine << Symbols::LF;
    out << contLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
