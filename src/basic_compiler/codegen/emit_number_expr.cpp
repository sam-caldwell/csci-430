// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_number_expr.cpp
 * Purpose: Implement CodeGenerator::emitNumberExpr (NumberExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/NumberExpr.h"
#include <format>
#include <sstream>
#include <string>

using namespace gwbasic;

/*
 * Function: emitNumberExpr
 * Summary: Render a numeric literal as a canonical floating value string.
 * Parameters:
 *  - out: IR output stream (unused).
 *  - num: Parsed NumberExpr node.
 * Returns:
 *  - std::string: Floating literal string (ensures decimal point or exponent).
 */
std::string CodeGenerator::emitNumberExpr(const std::ostringstream& out, const NumberExpr* num) {
    (void)out; // no IR emission needed for numeric literal
    // Use general floating format with 17 significant digits (like "%.17g").
    std::string formatted = std::format("{:.17g}", num->value);
    if (!formatted.contains('.') && !formatted.contains('e') && !formatted.contains('E')) {
        formatted += ".0";
    }
    return formatted;
}
