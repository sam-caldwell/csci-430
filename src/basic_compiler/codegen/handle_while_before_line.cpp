// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_while_before_line.cpp
 * Purpose: Implement CodeGenerator::handleWhileBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleWhileBeforeLine
 * Summary: Collect names referenced by WHILE before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleWhileBeforeLine(const Stmt* stmt,
                                          std::set<std::string, std::less<>>& vars,
                                          std::set<std::string, std::less<>>& arrays) {
    const auto* const whileStmt = dyn_cast<const WhileStmt>(stmt);
    if (whileStmt == nullptr) {
        return false;
    }

    collectVarsForBeforeLineFromExpr(whileStmt->cond.get(), vars, arrays);

    for (const auto& stmtItem : whileStmt->body) {
        collectVarsForBeforeLineFromStmt(stmtItem.get(), vars, arrays);
    }

    return true;
}
