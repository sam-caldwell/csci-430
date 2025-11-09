// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_print_before_line.cpp
 * Purpose: Implement CodeGenerator::handlePrintBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

bool CodeGenerator::handlePrintBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> &arrays) {
    const auto pr = dyn_cast<const PrintStmt>(s);
    if (!pr) return false;
    if (pr->value) collectVarsForBeforeLineFromExpr(pr->value.get(), vars, arrays);
    for (const auto &v: pr->more) collectVarsForBeforeLineFromExpr(v.get(), vars, arrays);
    return true;
}
