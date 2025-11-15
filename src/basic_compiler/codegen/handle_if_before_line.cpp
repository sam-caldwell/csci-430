// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_if_before_line.cpp
 * Purpose: Implement CodeGenerator::handleIfBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleIfBeforeLine
 * Summary: Collect names referenced by an IF condition before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleIfBeforeLine(const Stmt* stmt,
                                       std::set<std::string, std::less<>>& vars,
                                       std::set<std::string, std::less<>>& arrays) {
    const auto* const ifStmt = dyn_cast<const IfStmt>(stmt);
    if (ifStmt == nullptr) {
        return false;
    }
    collectVarsForBeforeLineFromExpr(ifStmt->cond.get(), vars, arrays);
    return true;
}
