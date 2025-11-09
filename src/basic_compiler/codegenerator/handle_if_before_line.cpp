// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_if_before_line.cpp
 * Purpose: Implement CodeGenerator::handleIfBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

bool CodeGenerator::handleIfBeforeLine(const Stmt *s,
                                       std::set<std::string, std::less<>> &vars,
                                       std::set<std::string, std::less<>> &arrays) {
    const auto is = dyn_cast<const IfStmt>(s);
    if (!is) return false;
    collectVarsForBeforeLineFromExpr(is->cond.get(), vars, arrays);
    return true;
}
