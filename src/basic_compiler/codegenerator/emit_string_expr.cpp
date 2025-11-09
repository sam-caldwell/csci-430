// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_string_expr.cpp
 * Purpose: Implement CodeGenerator::emitStringExpr (StringExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/StringExpr.h"
#include <format>
#include <sstream>
#include <string>

using namespace gwbasic;

std::string CodeGenerator::emitStringExpr(std::ostringstream& out, const StringExpr* stringExpr) {
    const int stringId = strLiteralId_[stringExpr->value];
    std::string gep = nextTemp();
    const std::string irLine = std::format(
        "  {} = getelementptr inbounds i8, ptr {}, i64 0", gep, globalStringName(stringId));
    out << irLine << Symbols::LF;
    std::ostringstream msg;
    msg << "line " << currentLine_ << " StringExpr -> " << irLine;
    log() << msg.str() << Symbols::LF;
    return gep;
}
