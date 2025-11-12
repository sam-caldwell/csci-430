// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: emitExpr
 * Summary: Dispatch expression lowering to specialized helpers and primitives.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - expr: Expression node to lower.
 *  - currBlockSuffix: Optional block suffix used for naming (unused here).
 * Returns:
 *  - std::string: Register name or literal with the resulting value.
 */
std::string CodeGenerator::emitExpr(std::ostringstream& out,
                                    const Expr* expr_ptr,
                                    [[maybe_unused]] const std::string& currBlockSuffix) {

    if (const auto* num = dyn_cast<const NumberExpr>(expr_ptr)) {
        return emitNumberExpr(out, num);
    }

    if (const auto* var = dyn_cast<const VarExpr>(expr_ptr)) {
        return emitVarExpr(out, var);
    }

    if (const auto* unary = dyn_cast<const UnaryExpr>(expr_ptr)) {
        return emitUnaryExpr(out, unary);
    }

    if (const auto* binary = dyn_cast<const BinaryExpr>(expr_ptr)) {
        return emitBinaryExpr(out, binary);
    }

    if (const auto* call = dyn_cast<const CallExpr>(expr_ptr)) {
        return emitCallExpr(out, call);
    }

    if (const auto* strExpr = dyn_cast<const StringExpr>(expr_ptr)) {
        const int strId = strLiteralId_[strExpr->value];
        std::string gep = nextTemp();
        out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", gep, globalStringName(strId)) << Symbols::LF;
        log() << std::format("line {} StringExpr -> gep", currentLine_) << Symbols::LF;
        return gep;
    }

    throw CodeGenError("Unknown expression kind");

}

} // namespace gwbasic
