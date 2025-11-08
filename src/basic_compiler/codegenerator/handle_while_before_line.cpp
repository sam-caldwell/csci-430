// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_while_before_line.cpp
 * Purpose: Implement CodeGenerator::handleWhileBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/WhileStmt.h"

using namespace gwbasic;

bool CodeGenerator::handleWhileBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> &arrays) {
    const auto ws = dyn_cast<const WhileStmt>(s);
    if (!ws) return false;
    collectVarsForBeforeLineFromExpr(ws->cond.get(), vars, arrays);
    for (const auto &st: ws->body) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
    return true;
}

