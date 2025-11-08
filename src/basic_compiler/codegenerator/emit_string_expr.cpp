// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_string_expr.cpp
 * Purpose: Implement CodeGenerator::emitStringExpr (StringExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/StringExpr.h"

using namespace gwbasic;

std::string CodeGenerator::emitStringExpr(std::ostringstream& out, const StringExpr* s) {
    int id = strLiteralId_[s->value];
    std::string gep = nextTemp();
    std::string ir = std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", gep, globalStringName(id));
    out << ir << Symbols::LF;
    std::ostringstream m; m << "line " << currentLine_ << " StringExpr -> " << ir; log() << m.str() << Symbols::LF;
    return gep;
}

