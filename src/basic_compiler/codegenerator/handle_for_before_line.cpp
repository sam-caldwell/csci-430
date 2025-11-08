// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_for_before_line.cpp
 * Purpose: Implement CodeGenerator::handleForBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ForStmt.h"

using namespace gwbasic;

bool CodeGenerator::handleForBeforeLine(const Stmt *s,
                                        std::set<std::string, std::less<>> &vars,
                                        std::set<std::string, std::less<>> &arrays) {
    const auto fs = dyn_cast<const ForStmt>(s);
    if (!fs) return false;
    vars.insert(fs->var);
    collectVarsForBeforeLineFromExpr(fs->start.get(), vars, arrays);
    collectVarsForBeforeLineFromExpr(fs->end.get(), vars, arrays);
    if (fs->step) collectVarsForBeforeLineFromExpr(fs->step.get(), vars, arrays);
    for (const auto &st: fs->body) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
    return true;
}

