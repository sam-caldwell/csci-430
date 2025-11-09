// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_number_expr.cpp
 * Purpose: Implement CodeGenerator::emitNumberExpr (NumberExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/NumberExpr.h"
#include <cstdio>

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
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.17g", num->value);
    std::string s(buf);
    if (s.find('.') == std::string::npos && s.find('e') == std::string::npos && s.find('E') == std::string::npos)
        s += ".0";
    return s;
}
