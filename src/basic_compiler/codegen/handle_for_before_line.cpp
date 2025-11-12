// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_for_before_line.cpp
 * Purpose: Implement CodeGenerator::handleForBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleForBeforeLine
 * Summary: Collect names referenced by a FOR header and body.
 * Parameters:
 *  - stmt: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleForBeforeLine(const Stmt* stmt,
                                        std::set<std::string, std::less<>>& vars,
                                        std::set<std::string, std::less<>>& arrays) {
    const auto* const forStmt = dyn_cast<const ForStmt>(stmt);
    if (forStmt == nullptr) {
        return false;
    }
    vars.insert(forStmt->var);
    collectVarsForBeforeLineFromExpr(forStmt->start.get(), vars, arrays);
    collectVarsForBeforeLineFromExpr(forStmt->end.get(), vars, arrays);
    if (forStmt->step) {
        collectVarsForBeforeLineFromExpr(forStmt->step.get(), vars, arrays);
    }
    for (const auto& bodyStmt : forStmt->body) {
        collectVarsForBeforeLineFromStmt(bodyStmt.get(), vars, arrays);
    }
    return true;
}
